using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DynamicView
{
    public delegate void StatusUpdated(object sender, StatusUpdatedEventArgs e);

    public class StatusUpdatedEventArgs : EventArgs
    {
        public Object status;
        public StatusUpdatedEventArgs(Object status)
        {
            this.status = status;
        }
    }

    public interface IDynamicViewDevice
    {
        /// <summary>
        /// Derived class must implement this to get device status
        /// </summary>
        public void GetStatusAsync();

        public Task<bool> ConnectAsync();

        /// <summary>
        /// Derived class must implement this to submit parameter changes
        /// </summary>
        public void Submit(String param, String value);

        public Task<ViewMetadata[]> GetMetaDataAsync();

        public event StatusUpdated Updated;
    }
}
