using emo;
using System;
using System.Threading.Tasks;

namespace bt
{
    internal class Program
    {
        private static ulong _BluetoothAddress;

        static async Task Main(string[] args)
        {
            Console.WriteLine($"Staring up! IsLittleEndian:{BitConverter.IsLittleEndian}");

            var _emo_ = new emoBT();

            if (true)
            {
                _emo_.NewEMOFound += newEMOFound;
                _emo_.NewEMOTextMessage += newEMOTextMessage;
                _emo_.NewEMOBinMessage += newEMOBinMessage;
                _emo_.startScanner();
            }

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
                    case ConsoleKey.NumPad1:
                        if (!await _emo_.SendAsync(_BluetoothAddress, _emo_.MakeCommandByteArray(0x00, new byte[] { (byte)(_IsCTRL_ ? 0x00 : 0x01) }))) Console.WriteLine("Failed to Send CMD1");
                        break;
                    case ConsoleKey.NumPad2:
                        if (!await _emo_.SendAsync(_BluetoothAddress, _emo_.MakeCommandByteArray(0x02, new byte[] { (byte)(_IsCTRL_ ? 0x00 : 0x01) }))) Console.WriteLine("Failed to Send CMD2");
                        break;
                    case ConsoleKey.NumPad4:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"request\":[0]},\"type\":\"sta_req\"}"))) Console.WriteLine("Failed to Send 'NumPad1'");
                        break;

                    case ConsoleKey.NumPad7:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"request\":[10]},\"type\":\"sta_req\"}"))) Console.WriteLine("Failed to Send 'NumPad7'");
                        break;
                    case ConsoleKey.NumPad8:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"op\":\"" + (_IsCTRL_ ? "out" : "in") + "\"},\"type\":\"anim_req\"}"))) Console.WriteLine("Failed to Send 'NumPad8'");
                        break;
                    case ConsoleKey.NumPad9:
                        if (!await _emo_.SendAsync(_BluetoothAddress, emoBT.Text2ByteArray("{\"data\":{\"name\":\"d1_EmoDance\",\"op\":\"play\"},\"type\":\"anim_req\"}"))) Console.WriteLine("Failed to Send 'NumPad9'");
                        break;
                }

            } while (_Key_.Key != ConsoleKey.Escape);

            _emo_.Dispose();
        }

        private static async void newEMOFound(object sender, NewEMOFoundEventArgs e)
        {
            var _EMO_ = (emoBT)sender;
            if (await _EMO_.connectEMOAsync(e.BluetoothAddress))
            {
                Console.WriteLine($"EMO {e.BluetoothAddress} connected!");
                _BluetoothAddress = e.BluetoothAddress;
                if (!await _EMO_.SendAsync(e.BluetoothAddress, _EMO_.MakeCommandByteArray(0x00, new byte[] { 0x01 }))) Console.WriteLine("Failed to Send CMD1");
                if (!await _EMO_.SendAsync(e.BluetoothAddress, _EMO_.MakeCommandByteArray(0x02, new byte[] { 0x01 }))) Console.WriteLine("Failed to Send CMD2");

                //if (!await _EMO_.SendAsync(e.BluetoothAddress, _EMO_.MakeCommandByteArray(0x01, new byte[] { 0x01 }))) Console.WriteLine("Failed to Send CMD4");
                //qif (!await _EMO_.SendAsync(e.BluetoothAddress, _EMO_.MakeCommandByteArray(0x03, new byte[] { 0x01 }))) Console.WriteLine("Failed to Send CMD5");
            }
            else
            {
                Console.WriteLine($"EMO {e.BluetoothAddress} failed to connect!");
            }
        }

        private static void newEMOTextMessage(object sender, NewEMOTextMessageEventArgs e)
        {
            Console.WriteLine($"{e.Message}");
        }

        private static void newEMOBinMessage(object sender, NewEMOBinMessageEventArgs e)
        {
            ParseBinMessage(e.Message);
        }

        private static void ParseBinMessage(byte[] Message)
        {
            Console.Write($"[{DateTime.Now}] ");
            var _StateType_ = BitConverter.ToInt16(new byte[] { Message[4], Message[3] });
            switch (_StateType_)
            {
                case 0x0000:
                    var _SubState_ = BitConverter.ToInt16(new byte[] { Message[6], Message[5] });

                    switch (_SubState_)
                    {
                        case 0x0100:
                            Console.WriteLine("Shaking");
                            break;

                        case 0x0200:
                            Console.WriteLine("Patting");
                            break;

                        case 0x0301:
                            Console.WriteLine("Lifted");
                            break;

                        case 0x0303:
                            Console.WriteLine("Charging");
                            break;

                        case 0x0400:
                            Console.WriteLine("Cliff emering");
                            break;

                        case 0x0501:
                            Console.WriteLine("Headset on/off");
                            break;

                        case 0x0601:
                            Console.WriteLine("Near wall");
                            break;

                        case 0x0A00:
                            Console.WriteLine("Play game animation");
                            break;

                        case 0x0B00:
                            Console.WriteLine("Listening");
                            break;

                        case 0x0C00:
                            Console.WriteLine("answering 1?");
                            break;

                        case 0x0E00:
                            Console.WriteLine("Danceing");
                            break;

                        case 0x0F00:
                            Console.WriteLine("Show Date/Time...");
                            break;

                        case 0x1100:
                            Console.WriteLine("Play Animation");
                            break;

                        case 0x1200:
                            Console.WriteLine("answering 2?");
                            break;

                        case 0x1700:
                            Console.WriteLine("Standing");
                            break;

                        case 0x1800:
                            Console.WriteLine("Sleeping");
                            break;

                        case 0x1900:
                            Console.WriteLine("Pated while sleeping?");
                            break;

                        case 0x1A00:
                            Console.WriteLine("Wakeup?");
                            break;

                        case 0x1D00:
                            Console.WriteLine("Exploring");
                            break;

                        case 0x1F00:
                            Console.WriteLine("Look arround?");
                            break;

                        case 0x2000:
                            Console.WriteLine("Stay idle on skateboard");
                            break;

                        case 0x2300:
                            Console.WriteLine("Play time of day animation");
                            break;

                        default:
                            Console.WriteLine($"StateType 0x{_StateType_:X4} - Unknown SubState 0x{_SubState_:X4} Parameters:{Convert.ToHexString(Message, 7, 13)}");
                            break;
                    }

                    break;
                case 0x0200:
                    Console.WriteLine($"Charge:{Message[5]}%");
                    break;

                default:
                    Console.WriteLine($"Unknown StateType 0x{_StateType_:X4} - {Convert.ToHexString(Message, 5, 15)}");
                    break;
            }
        }
    }
}
