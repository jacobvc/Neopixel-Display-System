using System.Collections.ObjectModel;
using System.Collections.Specialized;
using triaxis.BluetoothLE;
using System.Text;
using System.Text.Json;

namespace DynamicView;

public class NeopixelBleScanner
{
    private readonly IBluetoothLE _bluetooth;
    private NeopixelBleDeviceCollection _devices = new();
    private IDisposable? _scanner;

    public NeopixelBleScanner(IBluetoothLE bluetooth)
    {
        Devices = _devices;
        _bluetooth = bluetooth;
        _bluetooth.WhenAdapterChanges().Subscribe(OnAdapterChange);
    }

    private void OnAdapterChange(IAdapter adapter)
    {
        // stop previous scanner, if any
        _scanner?.Dispose();

        if (adapter.State == AdapterState.On)
        {
            // start scanning whenever Bluetooth is available
            var scanned = adapter.Scan(new ServiceUuid[] { new Uuid(NeopixelBleDevice.SERVICE_UUID)});
            _scanner = scanned.Subscribe(_devices.ProcessAdvertisement);
        }
        else
        {
            _scanner = null;
        }
    }
    public NeopixelBleDeviceCollection Devices { get; set; }
}
public class NeopixelBleDeviceCollection : ObservableCollection<NeopixelBleDevice>
{
    public void ProcessAdvertisement(IAdvertisement advertisement)
    {
        var dev = this.FirstOrDefault(d => d.Peripheral == advertisement.Peripheral);

        if (dev == null)
        {
            // add a new device
            dev = new NeopixelBleDevice(advertisement);
            Add(dev);
        }
        else
        {
            NeopixelBleDevice neo = dev as NeopixelBleDevice;
            if (neo.Update(advertisement))
            {
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Replace, dev, dev, IndexOf(dev)));
            }
        }
    }
}

public class NeopixelBleDevice : BleDevice, IDynamicViewDevice
{
    const String BLE_SERVER_NAME = "Neopixel BLE";
    public const String SERVICE_UUID = "991e4749-bdb3-4e79-96d8-9a1b20cd51ab";
    const String METADATA_CHARACTERISTIC_UUID = "9da99ba6-6a55-4bfd-aee8-401fdcd69bf5";
    const String STATUS_CHARACTERISTIC_UUID = "c0126900-f29f-4baf-8d8d-9e313d881339";

    ICharacteristic chStatus;
    ICharacteristic chMetadata;

    public event StatusUpdated Updated;

    public NeopixelBleDevice(IAdvertisement advertisement) : base(advertisement)
    {
    }

    public async Task<bool> ConnectAsync()
    {
        try
        {
            IPeripheralConnection periph = await Peripheral.ConnectAsync(1000);

            chMetadata = await periph.FindServiceCharacteristicAsync(
                new ServiceCharacteristicUuid(new ServiceUuid(new Uuid(SERVICE_UUID)),
                new CharacteristicUuid(new Uuid(METADATA_CHARACTERISTIC_UUID))));
            chStatus = await periph.FindServiceCharacteristicAsync(
                new ServiceCharacteristicUuid(new ServiceUuid(new Uuid(SERVICE_UUID)),
                new CharacteristicUuid(new Uuid(STATUS_CHARACTERISTIC_UUID))));
            //IObservable<byte[]> chStatusNotify = chStatus.Notifications();
            //chStatusNotify.Subscribe()
            return true;
        }
        catch (Exception ex)
        {
            Console.WriteLine("Connect failed: " + ex.Message);
            return false;
        }
    }

    /// <summary>
    /// Submit 'param' change to 'value' and get / update status
    /// </summary>
    /// <param name="param"></param>
    /// <param name="value"></param>
    public async void Submit(String param, String value)
    {
        await chStatus.WriteAsync(System.Text.Encoding.Default.GetBytes(
            "{ \"" + param + "\":\"" + value + "\" }"));

        GetStatusAsync();
    }

    /// <summary>
    /// Get / update status
    /// </summary>
    public async void GetStatusAsync()
    {
        string status = System.Text.Encoding.Default.GetString(await chStatus.ReadAsync());
        NeopixelStatus statusJson = JsonSerializer.Deserialize<NeopixelStatus>(status);
        UpdateStatus(statusJson);
    }

    /// <summary>
    /// Get metadata
    /// </summary>
    /// <returns></returns>
    public async Task<ViewMetadata[]> GetMetaDataAsync()
    {
        string meta = "";
        string part = "";
        byte i = 0;
        do
        {
            await chMetadata.WriteAsync(new byte[] { i++ });
            part = System.Text.Encoding.Default.GetString(await chMetadata.ReadAsync());
            meta += part;
        } while (part.Length > 0);
        return JsonSerializer.Deserialize<ViewMetadata[]>(meta);
    }
    /// <summary>
    /// Fire the status update event
    /// </summary>
    /// <param name="status"></param>
    protected void UpdateStatus(Object status)
    {
        Updated?.Invoke(this, new StatusUpdatedEventArgs(status));
    }

}

public class BleDevice
{
    public BleDevice(IAdvertisement advertisement)
    {
        Peripheral = advertisement.Peripheral;
        Update(advertisement);
    }

    public IPeripheral Peripheral { get; }

    public string? DeviceName { get; private set; }
    public int Rssi { get; private set; }
    public String Address { get; private set; }

    internal bool Update(IAdvertisement advertisement)
    {
        var rawName = advertisement[AdvertisementRecord.CompleteLocalName];
        var newName = rawName == null ? DeviceName : Encoding.ASCII.GetString(rawName);
#if ANDROID
        var newAddress = Peripheral.Uuid.RightHalf.ToString("X12");
#else
        var newAddress = Peripheral.Uuid.ToString();
#endif
        var newRssi = advertisement.Rssi;
        bool change = Address != newAddress; // DeviceName != newName || Rssi != newRssi;
        DeviceName = newName;
        Rssi = newRssi;
        Address = newAddress;
        return change;
    }

    public override string ToString()
    {
        return DeviceName + " (" + Address + ")";
    }
}

