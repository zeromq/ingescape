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
            int res = agent.init();

            if (res == 1)
            {
                Console.WriteLine(string.Format("IngeScape started with device : {0} and port : {1}", agent.GetDevice(), agent.GetPort()));
            }




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
                    Igs.startWithDevice("Wi-Fi",2009);
                }
            }
        }
    }
}
