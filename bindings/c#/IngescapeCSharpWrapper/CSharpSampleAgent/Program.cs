using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Ingescape;
using static System.Net.Mime.MediaTypeNames;

namespace CSharpSampleAgent
{
    class Program
    {
       
        static void Main(string[] args)
        {
            ConsoleKeyInfo cki;
            Console.Clear();

            Console.WriteLine(" ------------C# agent sample ------------");

            IgsAgent agent = new IgsAgent();
            agent.init();
            




            while (true)
            {
                Console.Write("Press 'X' to quit \n");
                Console.WriteLine("Press 'S' to stop \n");
                Console.WriteLine("Press 'R' to start \n");

                // Start a console read operation. Do not display the input.
                cki = Console.ReadKey(true);

                // Exit if the user pressed the 'X' key.
                if (cki.Key == ConsoleKey.X)
                {
                    Igs.stop();
                    Environment.Exit(0);
                    break;
                }
                if (cki.Key == ConsoleKey.S)
                {
                    Igs.stop();  
                }
                if (cki.Key == ConsoleKey.R)
                {
                    if(Igs.isStarted())
                    {
                        Igs.stop();
                    }                    
                    igs_result_t res = Igs.startWithDevice(agent.GetDevice(), agent.GetPort());
                    IgsAgent.printStartResult(res, agent.GetDevice(), agent.GetPort());
                }
            }
        }

       
    }
}
