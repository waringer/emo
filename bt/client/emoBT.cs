﻿using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using Windows.Devices.Bluetooth;
using Windows.Devices.Bluetooth.Advertisement;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Storage.Streams;

namespace emo
{
    public class emoBT : IDisposable
    {
        private BluetoothLEAdvertisementWatcher _Scanner;
        private List<ulong> _DeviceAdresses;
        private Dictionary<ulong, GattCharacteristic> _ConnectedDevices;
        private List<byte> _TextMessageBuffer;
        private byte _CommandCounter = 0;
        private bool _IsDisposed;

        public event EventHandler<EMOEventArgs> NewEMOFound;
        public event EventHandler<EMOEventArgs> EMODisconnected;
        public event EventHandler<EMOTextMessageEventArgs> NewEMOTextMessage;
        public event EventHandler<NewEMOBinMessageEventArgs> NewEMOBinMessage;

        public emoBT()
        {
            _DeviceAdresses = new List<ulong>();
            _ConnectedDevices = new Dictionary<ulong, GattCharacteristic>();
            _TextMessageBuffer = new List<byte>();
        }
        ~emoBT()
        {
            Dispose(disposing: false);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_IsDisposed)
            {
                if (disposing)
                {
                    if (_Scanner != null) _Scanner.Stop();
                    _DeviceAdresses.Clear();

                    try
                    {
                        foreach (var _Device_ in _ConnectedDevices)
                            _Device_.Value.Service.Device.Dispose();
                    }
                    catch { }
                }

                _IsDisposed = true;
            }
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        protected virtual void OnNewDeviceFound(EMOEventArgs e)
        {
            EventHandler<EMOEventArgs> handler = NewEMOFound;
            if (handler != null) handler(this, e);
        }

        protected virtual void OnEMODisconnected(EMOEventArgs e)
        {
            EventHandler<EMOEventArgs> handler = EMODisconnected;
            if (handler != null) handler(this, e);
        }

        protected virtual void OnNewEMOTextMessage(EMOTextMessageEventArgs e)
        {
            EventHandler<EMOTextMessageEventArgs> handler = NewEMOTextMessage;
            if (handler != null) handler(this, e);
        }

        protected virtual void OnNewEMOBinMessage(NewEMOBinMessageEventArgs e)
        {
            EventHandler<NewEMOBinMessageEventArgs> handler = NewEMOBinMessage;
            if (handler != null) handler(this, e);
        }

        public void startScanner()
        {
            _DeviceAdresses.Clear();
            _ConnectedDevices.Clear();
            _TextMessageBuffer.Clear();

            _Scanner = new BluetoothLEAdvertisementWatcher { ScanningMode = BluetoothLEScanningMode.Passive };
            _Scanner.Received += newDevice;
            _Scanner.Start();
            Console.WriteLine("Scanner started");
        }

        public void stopScanner()
        {
            if (_Scanner != null)
            {
                _Scanner.Stop();
                _Scanner = null;
                Console.WriteLine("Scanner stopped");
            }
        }

        public bool isScanning => _Scanner != null;

        public static byte[] Text2ByteArray(string Text)
        {
            var _bytes_ = new List<byte>();
            var _reqBytesText_ = Encoding.ASCII.GetBytes(Text);

            _bytes_.AddRange(new byte[] { 0xbb, 0xaa });
            _bytes_.AddRange(BitConverter.GetBytes((UInt16)_reqBytesText_.Length));
            _bytes_.AddRange(_reqBytesText_);
            return _bytes_.ToArray();
        }

        public byte[] MakeCommandByteArray(ushort Command, byte[] Parameters)
        {
            var _CommandBytes_ = BitConverter.GetBytes(Command);

            var _back_ = new byte[20];
            _back_[0] = 0xdd;
            _back_[1] = 0xcc;
            _back_[2] = _CommandCounter++;
            _back_[3] = _CommandBytes_[0];
            _back_[4] = _CommandBytes_[1];

            if (Parameters != null)
            {
                var _CopyLen_ = 15;
                if (Parameters.Length < _CopyLen_) _CopyLen_ = Parameters.Length;
                Array.Copy(Parameters, 0, _back_, 5, _CopyLen_);
            }

            return _back_;
        }

        public async Task<bool> connectEMOAsync(ulong BluetoothAddress)
        {
            var _EMO_ = await BluetoothLEDevice.FromBluetoothAddressAsync(BluetoothAddress);

            if (_EMO_ == null)
                return false;

            var _Gatt_ = await _EMO_.GetGattServicesAsync();
            foreach (var _Service_ in _Gatt_.Services)
            {
                Console.WriteLine($"\tService:{_Service_.Uuid}");

                var _Characteristics_ = await _Service_.GetCharacteristicsAsync();
                foreach (var _Characteristic_ in _Characteristics_.Characteristics)
                {
                    Console.WriteLine($"\t\tCharacteristic:{_Characteristic_.Uuid}");

                    var _Properties_ = _Characteristic_.CharacteristicProperties;
                    if (_Properties_.HasFlag(GattCharacteristicProperties.Notify))
                    {
                        var _Status_ = await _Characteristic_.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue.Notify);
                        if (_Status_ == GattCommunicationStatus.Success)
                        {
                            _Characteristic_.ValueChanged += newMessage;

                            _ConnectedDevices.Add(BluetoothAddress, _Characteristic_);
                            Console.WriteLine("\t\t\tSubscribed");

                            _EMO_.ConnectionStatusChanged += ConnectionStatusChanged;

                            return true;
                        }
                    }
                }
            }

            _EMO_.Dispose();
            return false;
        }

        private void ConnectionStatusChanged(BluetoothLEDevice sender, object args)
        {
            if (sender.ConnectionStatus == BluetoothConnectionStatus.Disconnected)
                OnEMODisconnected(new EMOEventArgs() { BluetoothAddress = sender.BluetoothAddress });
        }

        public async Task<bool> SendAsync(ulong BluetoothAddress, byte[] Buffer)
        {
            if (_ConnectedDevices.ContainsKey(BluetoothAddress))
            {
                var _Characteristic_ = _ConnectedDevices[BluetoothAddress];
                using (var _Writer_ = new DataWriter())
                {
                    _Writer_.WriteBytes(Buffer);
                    return (await _Characteristic_.WriteValueAsync(_Writer_.DetachBuffer(), GattWriteOption.WriteWithoutResponse) == GattCommunicationStatus.Success);
                }
            }

            return false;
        }

        private void newDevice(BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementReceivedEventArgs args)
        {
            if (!_DeviceAdresses.Contains(args.BluetoothAddress))
            {
                // Console.WriteLine($"Found device {args.BluetoothAddress} [{args.Advertisement.LocalName}]");

                if (args.Advertisement.LocalName.StartsWith("EMO-"))
                {
                    _DeviceAdresses.Add(args.BluetoothAddress);
                    OnNewDeviceFound(new EMOEventArgs() { BluetoothAddress = args.BluetoothAddress });
                    //Console.WriteLine($"{_device_.DeviceId}=>{_device_.BluetoothAddress} [{_device_.Name}] {RawSignalStrengthInDBm}");
                }
            }
        }

        private void newMessage(GattCharacteristic sender, GattValueChangedEventArgs args)
        {
            // An Indicate or Notify reported that the value has changed.
            using (var _Reader_ = DataReader.FromBuffer(args.CharacteristicValue))
            {
                var _Buffer_ = new byte[_Reader_.UnconsumedBufferLength];
                _Reader_.ReadBytes(_Buffer_);

                if (_Buffer_.Length > 3)
                {
                    if ((_Buffer_.Length == 20) && (_Buffer_[0] == 0xdd) && (_Buffer_[1] == 0xcc))
                    {
                        OnNewEMOBinMessage(new NewEMOBinMessageEventArgs() { BluetoothAddress = sender.Service.Device.BluetoothAddress, Message = _Buffer_ });
                        return;
                    }
                    else
                    {
                        if ((_Buffer_[0] == 0xbb) && (_Buffer_[1] == 0xaa))
                        {
                            _TextMessageBuffer.Clear();
                            _TextMessageBuffer.AddRange(_Buffer_);
                        }
                        else if (_TextMessageBuffer.Count > 0)
                            _TextMessageBuffer.AddRange(_Buffer_);
                    }
                }
                else
                {
                    if (_TextMessageBuffer.Count > 0)
                        _TextMessageBuffer.AddRange(_Buffer_);
                }

                if (_TextMessageBuffer.Count > 0)
                {
                    var _TextLen_ = BitConverter.ToInt16(new byte[] { _TextMessageBuffer[2], _TextMessageBuffer[3] });

                    if (_TextMessageBuffer.Count >= _TextLen_ + 4)
                    {
                        _TextMessageBuffer.RemoveRange(0, 4);
                        OnNewEMOTextMessage(new EMOTextMessageEventArgs() { BluetoothAddress = sender.Service.Device.BluetoothAddress, Message = Encoding.ASCII.GetString(_TextMessageBuffer.ToArray()) });
                        _TextMessageBuffer.Clear();
                    }
                }
            }
        }
    }

    public class EMOEventArgs : EventArgs
    {
        public ulong BluetoothAddress { get; set; }
    }

    public class EMOTextMessageEventArgs : EventArgs
    {
        public ulong BluetoothAddress { get; set; }
        public string Message { get; set; }
    }

    public class NewEMOBinMessageEventArgs : EventArgs
    {
        public ulong BluetoothAddress { get; set; }
        public byte[] Message { get; set; }
    }

    public enum emoActions
    {
        Being_shaken = 0x0100,
        Being_petted = 0x0200,
        Picked_up = 0x0301,
        Falls_down = 0x0302,
        Charging = 0x0303,
        Emerging_from_cliff = 0x0400,
        Headset_change = 0x0501,
        Low_power = 0x0502,
        Eye_Animation_Curious = 0x0503,
        Eye_Animation_Blink = 0x0504,
        Link_HomeStation = 0x0507,
        Obstacles_detected = 0x0601,
        Sound_detected = 0x0602,
        Shutdown = 0x0800,
        Playing_games = 0x0900,
        Having_fun = 0x0A00,
        Listening = 0x0B00,
        Speaking = 0x0C00,
        Playing_music = 0x0D00,
        Dancing = 0x0E00,
        Being_helpful = 0x0F00,
        Moving = 0x1000,
        Play_Animation = 0x1100,
        Answering_questions = 0x1200,
        Moving_to_target = 0x1300,
        Looking_at_face1 = 0x1400,
        Looking_at_face2 = 0x1500,
        Sees_nonhuman = 0x1600,
        Staying = 0x1700,
        Sleeping = 0x1800,
        Disturbed_while_sleeping = 0x1900,
        Wakeup = 0x1A00,
        Looking_around = 0x1B00,
        Exploring = 0x1D00,
        Searching = 0x1E00,
        Playing = 0x1F00,
        Stay_idle_on_skateboard = 0x2000,
        Standing_2 = 0x2100,
        Play_time_of_day_animation = 0x2300,
        Going_Home = 0x2400,
        Seek_HomeStation = 0x2500,
        Leave_Home = 0x2600,
    }
}
