using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Content
{
    [StructLayout(LayoutKind.Sequential)]
    public class PrimitiveDesc
    {
        public uint type;
        public float width;
        public float height;
        public float depth;
        public uint slicesX;
        public uint slicesY;
        public uint slicesZ;
        public uint LOD;
    }

    [StructLayout(LayoutKind.Sequential)]
    public class ImportParams
    {
        public float smoothingAngle;
        public byte bRH;
        public byte bGenerateNormals;
        public byte bGenerateTangents;
        public byte bImportTextures;
        public byte bImportAnimations;
        public byte reserved, reserved1;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public class GeometryData
    {
        public IntPtr data;
        public ulong size;
    }

    public static class CManager
    {
        [DllImport("ContentManager.dll")]
        public static extern void CreatePrimitive(PrimitiveDesc desc, ImportParams parameters, GeometryData data);
    }
}
