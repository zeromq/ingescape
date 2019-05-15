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

            Console.WriteLine(" ------------C# agent sample ------------");

            //Intanciate Igs agent middleware
            IgsAgent igsAgent = new IgsAgent();

            //Add inputs, output & parameter dynamically
            igsAgent.createDefDynamically();

            //Observe all inputs
            igsAgent.observeInputs();

            //Add mapping dynamically
            igsAgent.createMappingDynamically();

            //Write on outputs
            igsAgent.writeOnInputs();

            //Remove mapping dynamically after a while

            while (true)
            {
                Console.Write("Press  'X' to quit \n");

                // Start a console read operation. Do not display the input.
                cki = Console.ReadKey(true);

                // Exit if the user pressed the 'X' key.
                if (cki.Key == ConsoleKey.X)
                {
                    igsAgent.stop();
                    break;
                }
            }
        }
    }
}
