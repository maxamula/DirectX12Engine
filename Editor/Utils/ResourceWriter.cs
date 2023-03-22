using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Utils
{
    public class ResourceWriter
    {
        public static void WriteResource(string resource, string path)
        {
            var assembly = Assembly.GetExecutingAssembly();
            using (var inputStream = assembly.GetManifestResourceStream(resource))
            {
                try
                {
                    var outStream = File.OpenWrite(path);
                    inputStream.CopyTo(outStream);
                    outStream.Close();
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                }

            }
        }
    }
}