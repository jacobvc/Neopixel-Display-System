using System.Collections.ObjectModel;
using System.Collections.Specialized;
using triaxis.BluetoothLE;
using System.Text;

namespace DynamicView;

public class NeopixelBleScanner
{
    private readonly IBluetoothLE _bluetooth;
    private DynamicViewDeviceCollection _devices = new();
    private IDisposable? _scanner;

    public NeopixelBleScanner(IBluetoothLE bluetooth)
    {
        Console.WriteLine("Instantiating BluetoothScanner");
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
    public DynamicViewDeviceCollection Devices { get; set; }
}
public class DynamicViewDeviceCollection : ObservableCollection<IDynamicViewDevice>
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

