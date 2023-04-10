using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Media.Media3D;
using Engine;
using NumericUpDownLib;

namespace Editor.Components
{
    [DataContract]
    public class Transformation : Component
    {
        private Engine.Transformation _engineTransformation;
        public Transformation(GameObject parent) : base(parent)
        {
            _engineTransformation = Object._engineObject.GetTransformation();
            _x = X; _y = Y; _z = Z;
            _rotx = RotX; _roty = RotY; _rotz = RotZ;
            _scalex = ScaleX; _scaley = ScaleY; _scalez = ScaleZ;
        }
        public override void ApplyToEngine()
        {
            _engineTransformation = Object._engineObject.GetTransformation();
            X = _x; RotX = _rotx; ScaleX = _scalex;
            Y = _y; RotY = _roty; ScaleY = _scaley;
            Z = _z; RotZ = _rotz; ScaleZ = _scalez;
        }

        [DataMember] float _x = 0;
        public float X
        {
            get => _engineTransformation.X;
            set
            {
                _engineTransformation.X = value;
                _x = value;
                OnPropertyChanged(nameof(X));
            }
        }
        [DataMember] float _y = 0;
        public float Y
        {
            get => _engineTransformation.Y;
            set
            {
                _engineTransformation.Y = value;
                _y = value;
                OnPropertyChanged(nameof(Y));
            }
        }
        [DataMember] float _z = 0;
        public float Z
        {
            get => _engineTransformation.Z;
            set
            {
                _engineTransformation.Z = value;
                _z = value;
                OnPropertyChanged(nameof(Z));
            }
        }

        [DataMember] float _rotx = 0;
        public float RotX
        {
            get => _engineTransformation.RotX;
            set
            {
                _engineTransformation.RotX = value;
                _rotx = value;
                OnPropertyChanged(nameof(RotX));
            }
        }
        [DataMember] float _roty = 0;
        public float RotY
        {
            get => _engineTransformation.RotY;
            set
            {
                _engineTransformation.RotY = value;
                _roty = value;
                OnPropertyChanged(nameof(RotY));
            }
        }
        [DataMember] float _rotz = 0;
        public float RotZ
        {
            get => _engineTransformation.RotZ;
            set
            {
                _engineTransformation.RotZ = value;
                _rotz = value;
                OnPropertyChanged(nameof(RotZ));
            }
        }

        [DataMember] float _scalex = 0;
        public float ScaleX
        {
            get => _engineTransformation.ScaleX;
            set
            {
                _engineTransformation.ScaleX = value;
                _scalex = value;
                OnPropertyChanged(nameof(ScaleX));
            }
        }
        [DataMember] float _scaley = 0;
        public float ScaleY
        {
            get => _engineTransformation.ScaleY;
            set
            {
                _engineTransformation.ScaleY = value;
                _scaley = value;
                OnPropertyChanged(nameof(ScaleY));
            }
        }
        [DataMember] float _scalez = 0;
        public float ScaleZ
        {
            get => _engineTransformation.ScaleZ;
            set
            {
                _engineTransformation.ScaleZ = value;
                _scalez = value;
                OnPropertyChanged(nameof(ScaleZ));
            }
        }
    }
}
