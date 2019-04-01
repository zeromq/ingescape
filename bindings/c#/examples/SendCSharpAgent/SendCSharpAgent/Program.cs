using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using IngescapeCSharp;

namespace SendCSharpAgent
{
    class Program
    {


        static void Main(string[] args)
        {
            ConsoleKeyInfo cki;
            Console.Clear();

            Console.WriteLine(" ------------Send c# agent example ------------");

            Ingescape.igs_startWithDevice("Ethernet", 5670);

            while (true)
            {
                Console.Write("Press  'X' to quit \n");

                // Start a console read operation. Do not display the input.
                cki = Console.ReadKey(true);

                // Exit if the user pressed the 'X' key.
                if (cki.Key == ConsoleKey.X)
                {
                    Ingescape.igs_stop();
                    break;
                }
            }
        }
    }
}
