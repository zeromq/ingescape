using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Ingescape;

namespace CSharpSampleAgent
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



            //Add mapping dynamically
            igsAgent.createMappingDynamically();

            //Write on outputs
            igsAgent.writeOnInputs();

            //Observe all inputs
            igsAgent.observeInputs();

            //Read inputs
            igsAgent.readGenericFunctions();

            //Write & Read data
            igsAgent.writeAndReadData();

            int iteration = 0;
            string msg = null;
            while (true)
            {
                Console.Write("Press  'X' to quit \n Press  'A' to write in log \n");

                // Start a console read operation. Do not display the input.
                cki = Console.ReadKey(true);

                // Exit if the user pressed the 'X' key.
                if (cki.Key == ConsoleKey.X)
                {
                    igsAgent.stop();
                    break;
                }

                // Exit if the user pressed the 'A' key.
                if (cki.Key == ConsoleKey.A)
                {
                    //Loop of potential functions raising memory leaks
                    //Write input type int, double, impulsion, string 1000 times
                    //The memory increase. it seems to have no relation with the c# but more with the C implementation
                    igsAgent.memoryleakstest();
                }
            }
        }
    }
}
