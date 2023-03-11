using Editor.Controls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Editor
{
    [StructLayout(LayoutKind.Sequential)]
    public class PrimitiveDesc
    {
        public Content.PrimitiveType type = Content.PrimitiveType.Plane;
        public Vector3 size = new Vector3(1f);
        public int segmentX = 1;
        public int segmentY = 1;
        public int segmentZ = 1;
        public int LOD = 0;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class ImportParams
    {
        public float smoothingAngle = 178f;
        public byte bRH = 1;
        public byte bGenerateNormals = 1;
        public byte bGenerateTangents = 1;
        public byte bImportTextures = 1;
        public byte bImportAnimations = 1;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class GeometryData : IDisposable
    {
        public IntPtr data;
        public ulong size;

        public void Dispose()
        {
            Marshal.FreeCoTaskMem(data);
            GC.SuppressFinalize(this);
        }

        ~GeometryData()
        {
            Dispose();
        }
    }

    static public class ContentManager
    {
        private const string _lib = "ContentManager.dll";
        [DllImport(_lib)]
        private static extern void CreatePrimitive(PrimitiveDesc desc, ImportParams importParams, GeometryData data);
        public static void CreatePrimitive(Content.Geometry geometry, PrimitiveDesc desc, ImportParams importParams)
        {
            Debug.Assert(geometry != null);
            using var geomData = new GeometryData();
            try
            {
                CreatePrimitive(desc, importParams, geomData);
                Debug.Assert(geomData.data != IntPtr.Zero);
                var data = new byte[geomData.size];
                Marshal.Copy(geomData.data, data, 0, data.Length);
                geometry.FromRawData(data);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }
    }
}
