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

        public static void WriteFolder(string resource, string path)
        {
            var assembly = Assembly.GetExecutingAssembly();
            string[] resourceNames = assembly.GetManifestResourceNames();

            foreach (string resourceName in resourceNames)
            {
                if (resourceName.StartsWith(resource))
                {
                    string fileName = resourceName.Substring(resource.Length + 1);
                    string filePath = Path.Combine(path, fileName);

                    using (var inputStream = assembly.GetManifestResourceStream(resourceName))
                    {
                        try
                        {
                            var outStream = File.OpenWrite(filePath);
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
    }
}