using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Components
{
    [DataContract]
    class Script : Component
    {
        public Script(GameObject parent) : base(parent)
        {    }

        public override void WriteBin(BinaryWriter bw)
        {
            bw.Write(_id);
        }
        public override ComponentType GetComponentType() => ComponentType.Script;

        [DataMember] private string _name;
        public string Name
        {
            get => _name;
            set
            {
                _name = value;
                OnPropertyChanged(nameof(Name));
            }
        }

        [DataMember] private ulong _id;
        public ulong ID
        {
            get => _id;
            set
            {
                _id = value;
                OnPropertyChanged(nameof(ID));
            }
        }
    }
}
