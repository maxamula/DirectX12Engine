using Editor.Utils;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Project
{
    [DataContract(Name = "Project")]
    public class Project : VMBase
    {
        [DataMember] public string Name { get; private set; }
        [DataMember] public string Path { get; private set; }

    }
}
