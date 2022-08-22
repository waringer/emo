using emo;
using System;
using System.Threading.Tasks;

namespace bt
{
    internal class Program
    {
        private static ulong _BluetoothAddress;
        private static bool _ChargeSub = false;
        private static bool _ActionSub = false;

        static async Task Main(string[] args)
        {
            Console.WriteLine($"Staring up! IsLittleEndian:{BitConverter.IsLittleEndian}");

            var _emo_ = new emoBT();
            _emo_.NewEMOFound += newEMOFound;
            _emo_.EMODisconnected += EMODisconnected;
            _emo_.NewEMOTextMessage += newEMOTextMessage;
            _emo_.NewEMOBinMessage += newEMOBinMessageAsync;
            _emo_.startScanner();

            Console.WriteLine("Wait for key... [ESC] to Quit; [Q] to Shutdown Emo");
            ConsoleKeyInfo _Key_;
            do
            {
                _Key_ = Console.ReadKey();
                var _IsCTRL_ = ((_Key_.Modifiers & ConsoleModifiers.Control) != 0);


                switch (_Key_.Key)
                {
                    case ConsoleKey.Q:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"type\":\"off_req\"}"))) Console.WriteLine("Failed to Send 'Quit'");
                        break;
						
                    case ConsoleKey.U:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"num\":23,\"type\":\"ota\"}"))) Console.WriteLine("Failed to Send 'Update'");
                        break;

                    case ConsoleKey.NumPad1:
                        if (!await _emo_.SendAsync(_BluetoothAddress, _emo_.MakeCommandByteArray(0x00, new byte[] { (byte)(_IsCTRL_ ? 0x00 : 0x01) }))) Console.WriteLine("Failed to Send 'subscribe actions'");
                        else _ActionSub = true;
                        break;
                    case ConsoleKey.NumPad2:
                        if (!await _emo_.SendAsync(_BluetoothAddress, _emo_.MakeCommandByteArray(0x02, new byte[] { (byte)(_IsCTRL_ ? 0x00 : 0x01) }))) Console.WriteLine("Failed to Send 'unsubscribe actions'");
                        else _ActionSub = false;
                        break;

                    case ConsoleKey.NumPad4:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"request\":[0]},\"type\":\"sta_req\"}"))) Console.WriteLine("Failed to Send 'status request 0 (device id)'");
                        break;

                        // case ConsoleKey.NumPad7:
                        //     if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"request\":[10]},\"type\":\"sta_req\"}"))) Console.WriteLine("Failed to Send 'status request 10 (dances)'");
                        //     break;

                        // case ConsoleKey.NumPad8:
                        //     if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"op\":\"" + (_IsCTRL_ ? "out" : "in") + "\"},\"type\":\"anim_req\"}"))) Console.WriteLine("Failed to Send 'NumPad8'");
                        //     break;
                        // case ConsoleKey.NumPad9:
                        //     if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"name\":\"d1_EmoDance\",\"op\":\"play\"},\"type\":\"anim_req\"}"))) Console.WriteLine("Failed to Send 'NumPad9'");
                        //     break;
                }

            } while (_Key_.Key != ConsoleKey.Escape);

            _emo_.Dispose();
        }

        private static async void newEMOFound(object sender, EMOEventArgs e)
        {
            var _EMO_ = (emoBT)sender;
            _EMO_.stopScanner();
            if (await _EMO_.connectEMOAsync(e.BluetoothAddress))
            {
                Console.WriteLine($"EMO {e.BluetoothAddress:X} connected!");
                _BluetoothAddress = e.BluetoothAddress;

                if (!_ChargeSub)
                    if (!await _EMO_.SendAsync(e.BluetoothAddress, _EMO_.MakeCommandByteArray(0x02, new byte[] { 0x01 }))) Console.WriteLine("Failed to Send 'subscribe charge'");
                    else _ChargeSub = true;
            }
            else
            {
                Console.WriteLine($"EMO {e.BluetoothAddress} failed to connect!");
                _EMO_.startScanner();
            }
        }

        private static void EMODisconnected(object sender, EMOEventArgs e)
        {
            var _EMO_ = (emoBT)sender;
            _EMO_.startScanner();
        }

        private static void newEMOTextMessage(object sender, EMOTextMessageEventArgs e) => Console.WriteLine($"{e.Message}");

        private static async void newEMOBinMessageAsync(object sender, NewEMOBinMessageEventArgs e) => await ParseBinMessageAsync((emoBT)sender, e.Message);

        private static async Task ParseBinMessageAsync(emoBT emo, byte[] Message)
        {
            Console.Write($"[{DateTime.Now}] ");
            var _StateType_ = BitConverter.ToInt16(new byte[] { Message[4], Message[3] });
            switch (_StateType_)
            {
                case 0x0000: // Action
                    var _ActionType_ = BitConverter.ToInt16(new byte[] { Message[6], Message[5] });

                    if (Enum.IsDefined(typeof(emoActions), (int)_ActionType_))
                        Console.WriteLine($"{((emoActions)_ActionType_).ToString().Replace('_', ' ')}");
                    else
                        Console.WriteLine($"StateType 0x{_StateType_:X4} - Unknown SubState 0x{_ActionType_:X4} Parameters:{Convert.ToHexString(Message, 7, 13)}");

                    break;
                case 0x0200: // Charge
                    Console.WriteLine($"Charge:{Message[5]}%");

                    if (!_ActionSub)
                        if (!await emo.SendAsync(_BluetoothAddress, emo.MakeCommandByteArray(0x00, new byte[] { 0x01 }))) Console.WriteLine("Failed to Send 'subscribe actions'");
                        else _ActionSub = true;


                    break;

                default:
                    Console.WriteLine($"Unknown StateType 0x{_StateType_:X4} - {Convert.ToHexString(Message, 5, 15)}");
                    break;
            }
        }
    }
}
