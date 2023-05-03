using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Editor.Content
{
    public enum AssetType
    {
        Unknown,
        Animation,
        Audio,
        Material,
        Mesh,
        Sleleton,
        Texture,
    }

    public sealed class AssetInfo
    {
        public AssetType Type { get; set; }
        public byte[] Icon { get; set; }
        public string FullPath { get; set; }
        public string SourcePath { get; set; }
        public string FileName => Path.GetFileNameWithoutExtension(FullPath);
        public DateTime ImportDate { get; set; }
        public Guid Guid { get; set; }
    }

    abstract public class Asset : VMBase
    {
        public static AssetInfo GetAssetInfo(string file)
        {
            Debug.Assert(File.Exists(file));
            try
            {
                using var br = new BinaryReader(new FileStream(file, FileMode.Open, FileAccess.Read));
                var info = GetAssetInfo(br);
                info.FullPath = file;
                return info;
            }
            catch (Exception ex) 
            {
                Debug.WriteLine(ex.Message);
                return null;
            }
        }
        public static AssetInfo GetAssetInfo(BinaryReader br)
        {
            br.BaseStream.Position = 0;
            AssetInfo info = new AssetInfo();
            info.Type = (AssetType)br.ReadInt32();
            var idSize = br.ReadInt32();
            info.Guid = new Guid(br.ReadBytes(idSize));
            info.ImportDate = DateTime.FromBinary(br.ReadInt64());
            info.SourcePath = br.ReadString();
            var iconSize = br.ReadInt32();
            info.Icon = br.ReadBytes(iconSize);
            return info;
        }
        public Asset(AssetType assetType)
        {
            Debug.Assert(assetType != AssetType.Unknown);
            Type = assetType;
        }

        public abstract IEnumerable<string> Save(string file);

        public AssetType Type { get; private set; }
        public byte[] Icon { get; protected set; }
        public string SourcePath { get; protected set; }
        private string _fullpath;
        public string FullPath
        {
            get => _fullpath;
            set
            {
                _fullpath = value;
                OnPropertyChanged(nameof(FullPath));
                OnPropertyChanged(nameof(FileName));
            }
        }
        public string FileName => Path.GetFileNameWithoutExtension(FullPath);

        public Guid Guid { get; protected set; } = Guid.NewGuid();
        public DateTime ImportTime { get; protected set; }
        public byte[] Hash { get; protected set; }
        protected void _WriteAssetHeader(BinaryWriter bw)
        {
            bw.BaseStream.Position = 0;

            var id = Guid.ToByteArray();
            var date = DateTime.Now.ToBinary();

            bw.Write((uint)Type);
            bw.Write(id.Length);
            bw.Write(id);
            bw.Write(date);
            bw.Write(SourcePath ?? "");
            bw.Write(Icon.Length);
            bw.Write(Icon);
        }
    }
}
