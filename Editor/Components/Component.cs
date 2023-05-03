using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Components
{
    public enum ComponentType
    {
        Transfoormation,
        Script,
    }

    [KnownType(typeof(Transformation))]
    [DataContract]
    public abstract class Component : VMBase
    {
        public Component(GameObject parent)
        {
            Object = parent;
        }

        virtual public void ApplyToEngine()
        { }

        abstract public void WriteBin(BinaryWriter bw);
        abstract public ComponentType GetComponentType();


        [DataMember]
        public GameObject Object
        {
            get => _object;
            set
            {
                if (_object != value)
                {
                    _object = value;
                    OnPropertyChanged(nameof(Object));
                }
            }
        }
        private GameObject _object;
    }
}
