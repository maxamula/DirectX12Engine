using System;
using System.Collections.Generic;
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

        [DataMember] private long _id;
    }
}
