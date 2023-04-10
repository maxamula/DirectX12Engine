using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Content
{
    public enum PrimitiveType
    {
        Plane,
        Box,
        Sphere,
        Count
    };

    public class Mesh : VMBase
    {
        private int _vertexSize;
        public int VertexSize
        {
            get => _vertexSize;
            set
            {
                if (_vertexSize != value)
                {
                    _vertexSize = value;
                    OnPropertyChanged(nameof(VertexSize));
                }
            }
        }
        public int _vertexCount;
        public int VertexCount
        {
            get => _vertexCount;
            set
            {
                if (_vertexCount != value)
                {
                    _vertexCount = value;
                    OnPropertyChanged(nameof(VertexCount));
                }
            }
        }
        public int _indexSize;
        public int IndexSize
        {
            get => _indexSize;
            set
            {
                if (_indexSize != value)
                {
                    _indexSize = value;
                    OnPropertyChanged(nameof(IndexSize));
                }
            }
        }
        public int _indexCount;
        public int IndexCount
        {
            get => _indexCount;
            set
            {
                if (_indexCount != value)
                {
                    _indexCount = value;
                    OnPropertyChanged(nameof(IndexCount));
                }
            }
        }

        public byte[] Vertices { get; set; }
        public byte[] Indices { get; set; }
    }

    public class MeshLOD : VMBase
    {
        private string _name;
        public string Name
        {
            get => _name;
            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
        private float _lodThreshold;
        public float LodThreshold
        {
            get => _lodThreshold;
            set
            {
                if (_lodThreshold != value)
                {
                    _lodThreshold = value;
                    OnPropertyChanged(nameof(LodThreshold));
                }
            }
        }
        public ObservableCollection<Mesh> Meshes { get; } = new ObservableCollection<Mesh>();
    }

    public class LODGroup : VMBase
    {
        private string _name;
        public string Name
        {
            get => _name;
            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
        public ObservableCollection<MeshLOD> LODs { get; } = new ObservableCollection<MeshLOD>();
    }
    public class Geometry : Asset
    {
        public Geometry() : base(AssetType.Mesh)
        {

        }

        public void FromRawData(byte[] data)
        {
            _lodGroups.Clear();
            var reader = new BinaryReader(new MemoryStream(data));
            // Scene name
            var s = reader.ReadInt32();
            reader.BaseStream.Position += s;
            // num LODs
            var numGroups = reader.ReadInt32();
            Debug.Assert(numGroups > 0);
            for (int i = 0; i < numGroups; i++)
            {
                // group name
                s = reader.ReadInt32();
                string lodGroupName;
                if (s > 0)
                {
                    var nameBytes = reader.ReadBytes(s);
                    lodGroupName = Encoding.UTF8.GetString(nameBytes);
                }
                else
                {
                    lodGroupName = $"lod_{Guid.NewGuid().ToString().Substring(0, 6)}";
                }

                //num meshes
                var numMeshes = reader.ReadInt32();
                Debug.Assert(numMeshes > 0);
                List<MeshLOD> lods = _ReadMeshLODs(numMeshes, reader);

                var lodGroup = new LODGroup() { Name = lodGroupName };
                lods.ForEach(l => lodGroup.LODs.Add(l));
                _lodGroups.Add(lodGroup);
            }
            reader.Close();
        }

        private static List<MeshLOD> _ReadMeshLODs(int numMeshes, BinaryReader reader)
        {
            var lodIds = new List<int>();
            var lodList = new List<MeshLOD>();
            for (int i = 0; i < numMeshes; i++)
            {
                _ReadMeshes(reader, lodIds, lodList);
            }
            return lodList;
        }

        private static void _ReadMeshes(BinaryReader reader, List<int> lodIds, List<MeshLOD> lodList)
        {
            // Mesh name
            var s = reader.ReadInt32();
            string meshName;
            if (s > 0)
            {
                var nameBytes = reader.ReadBytes(s);
                meshName = Encoding.UTF8.GetString(nameBytes);
            }
            else
            {
                meshName = $"mesh_{Guid.NewGuid().ToString().Substring(0, 6)}";
            }

            var mesh = new Mesh();

            var lodId = reader.ReadInt32();
            mesh.VertexSize = reader.ReadInt32();
            mesh.VertexCount = reader.ReadInt32();
            mesh.IndexSize = reader.ReadInt32();
            mesh.IndexCount = reader.ReadInt32();
            var lodThreshold = reader.ReadSingle();

            var vertrBufSize = mesh.VertexSize * mesh.VertexCount;
            var indexBufSize = mesh.IndexSize * mesh.IndexCount;

            mesh.Vertices = reader.ReadBytes((int)vertrBufSize);
            mesh.Indices = reader.ReadBytes((int)indexBufSize);

            MeshLOD lod;
            if (lodIds.Contains(lodId))
            {
                lod = lodList[lodIds.IndexOf(lodId)];
                Debug.Assert(lod != null);
            }
            else
            {
                lodIds.Add(lodId);
                lod = new MeshLOD() { Name = meshName, LodThreshold = lodThreshold };
                lodList.Add(lod);
            }
            lod.Meshes.Add(mesh);
        }

        public LODGroup GetLODGroup(int lodGroup = 0)
        {
            Debug.Assert(lodGroup >= 0 && lodGroup < _lodGroups.Count);
            return _lodGroups.Any() ? _lodGroups[lodGroup] : null;
        }

        private readonly List<LODGroup> _lodGroups = new List<LODGroup>();
    }
}
