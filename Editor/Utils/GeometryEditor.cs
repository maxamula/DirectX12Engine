using Editor.Content;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;
using System.Windows.Media;
using System.Collections.ObjectModel;
using System.IO;

namespace Editor.Utils
{
    public class MeshVertexData : VMBase
    {
        private Brush _specular = new SolidColorBrush((Color)ColorConverter.ConvertFromString("#ff111111"));
        public Brush Specular
        {
            get => _specular;
            set
            {
                if (_specular != value)
                {
                    _specular = value;
                    OnPropertyChanged(nameof(Specular));
                }
            }
        }

        private Brush _diffuse = Brushes.White;
        public Brush Diffuse
        {
            get => _diffuse;
            set
            {
                if (_diffuse != value)
                {
                    _diffuse = value;
                    OnPropertyChanged(nameof(Diffuse));
                }
            }
        }

        public Point3DCollection Positions { get; } = new Point3DCollection();
        public Vector3DCollection Normals { get; } = new Vector3DCollection();
        public PointCollection UVs { get; } = new PointCollection();
        public Int32Collection Indices { get; } = new Int32Collection();
    }

    public class MeshRenderer : VMBase
    {
        public MeshRenderer(MeshLOD lod, MeshRenderer o)
        {
            if(o != null)
            {
                this.CameraDir = o.CameraDir;
                this.CameraPos = o.CameraPos;
                this.CameraTarg = o.CameraTarg;
            }
            foreach (var m in lod.Meshes)
            {
                var mask = m.VertexComponents;
                var vertexCount = m.VertexCount;
                var indexSize = m.IndexSize;
                var indexCount = m.IndexCount;

                BinaryReader reader = new BinaryReader(new MemoryStream(m.Vertices));
                MeshVertexData data = new MeshVertexData();
                for(int i =0; i < vertexCount; i++)
                {
                    data.Positions.Add(new Point3D(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle()));
                    if ((mask & (uint)ElementType.Normal) != 0) data.Normals.Add(new Vector3D(reader.ReadSingle(), reader.ReadSingle(), reader.ReadSingle()));
                    if ((mask & (uint)ElementType.Tangent) != 0) reader.BaseStream.Position += 4 * sizeof(float);
                    if ((mask & (uint)ElementType.Texcoord) != 0) data.UVs.Add(new System.Windows.Point(reader.ReadSingle(), reader.ReadSingle()));
                    if ((mask & (uint)ElementType.Color) != 0) reader.BaseStream.Position += 3 * sizeof(byte);
                }
                reader.Close();
                reader = new BinaryReader(new MemoryStream(m.Indices));
                for(int i = 0; i < indexCount; i++)
                {
                    if(indexSize == sizeof(ushort))
                        data.Indices.Add((int)reader.ReadUInt16());
                    else
                        data.Indices.Add((int)reader.ReadUInt32());
                }
                reader.Close();
                Meshes.Add(data);
            }
        }
        public ObservableCollection<MeshVertexData> Meshes { get; } = new ObservableCollection<MeshVertexData>();

        private Vector3D _cameraDir = new Vector3D(0, 0, -1);
        public Vector3D CameraDir
        {
            get => _cameraDir;
            set
            {
                if (_cameraDir != value)
                {
                    _cameraDir = value;
                    OnPropertyChanged(nameof(CameraDir));
                }
            }
        }

        private Point3D _cameraPos = new Point3D(0, 0, 10);
        public Point3D CameraPos
        {
            get => _cameraPos;
            set
            {
                if (_cameraPos != value)
                {
                    _cameraPos = value;
                    CameraDir = new Vector3D(-value.X, -value.Y,-value.Z);
                    OnPropertyChanged(nameof(CameraPos));
                    OnPropertyChanged(nameof(OffsetCameraPos));
                }
            }
        }

        private Point3D _cameraTarg = new Point3D(0, 0, 0);
        public Point3D CameraTarg
        {
            get => _cameraTarg; 
            set
            {
                if (_cameraTarg != value)
                {
                    _cameraTarg = value;
                    OnPropertyChanged(nameof(CameraTarg));
                    OnPropertyChanged(nameof(OffsetCameraPos));
                }
            }
        }

        public Point3D OffsetCameraPos => new Point3D(CameraPos.X + CameraTarg.X, CameraPos.Y + CameraTarg.Y, CameraPos.Z + CameraTarg.Z);

        private Color _keyLight = Color.FromArgb(255, 255, 255, 255);
        public Color KeyLight
        {
            get => _keyLight;
            set
            {
                if(_keyLight != value)
                {
                    _keyLight = value;
                    OnPropertyChanged(nameof(KeyLight));
                }
            }
        }
    }

    public class GeometryEditor : VMBase, IAssetEditor
    {
        public Asset Asset => _geometry;

        private Content.Geometry _geometry;
        public Content.Geometry Geometry
        {
            get => _geometry;
            set
            {
                if (_geometry != value) 
                {
                    _geometry = value;
                    OnPropertyChanged(nameof(Geometry));
                }
            }
        }

        private MeshRenderer _renderer;
        public MeshRenderer Renderer
        {
            get => _renderer;
            set
            {
                if(_renderer != value)
                {
                    _renderer = value;
                    OnPropertyChanged(nameof(Renderer));
                }
            }
        }

        public void SetAsset(Asset asset)
        {
            if(asset is Content.Geometry geometry)
            {
                Geometry = geometry;
                Renderer = new MeshRenderer(geometry.GetLODGroup().LODs[0], Renderer);
            }
        }
    }
}
