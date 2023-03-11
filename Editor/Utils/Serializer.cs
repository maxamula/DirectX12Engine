using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Utils
{
    public class Serializer
    {
        public static void Serialize<T>(T instance, string path)   // Write memory structures to file
        {
            try
            {
                using (var fileStream = new FileStream(path, FileMode.OpenOrCreate, FileAccess.ReadWrite))
                {
                    var serializer = new DataContractSerializer(typeof(T));
                    fileStream.SetLength(0);
                    serializer.WriteObject(fileStream, instance);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }

        public static T Deserialize<T>(string path)
        {
            try
            {
                using (var fileStream = new FileStream(path, FileMode.Open))
                {
                    var serializer = new DataContractSerializer(typeof(T));
                    T instance = (T)serializer.ReadObject(fileStream);
                    return instance;
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                return default(T);
            }
        }
    }
}
