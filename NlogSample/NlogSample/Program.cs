using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NLog;

namespace NlogSample
{
    class Program
    {
        private static readonly Logger logger = LogManager.GetCurrentClassLogger();

        static void Main(string[] args)
        {
            string line = Console.ReadLine();
            while(line != "exit")
            {
                logger.Info(":" + line + "\n");
                line = Console.ReadLine();
            }
        }
    }
}
