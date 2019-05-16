using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using IngescapeCSharp;

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

            //Observe all inputs
            igsAgent.observeInputs();

            //Add mapping dynamically
            igsAgent.createMappingDynamically();

            //Write on outputs
            igsAgent.writeOnInputs();

            //Read inputs
            igsAgent.readGenericFunctions();

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
                    msg = "log : " + iteration++;
                    igsAgent.writeInLog(msg);
                }
            }
        }
    }
}
