using Editor.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace Editor.Project
{
    [DataContract(Name = "Project")]
    public class Project : VMBase
    {
        public static Project Load(string file)
        {
            Debug.Assert(File.Exists(file));
            return Utils.Serializer.Deserialize<Project>(file);
        }
        public void Unload()
        {
            // TODO close vs
        }
        public static Project Current { get => Application.Current.MainWindow.DataContext as Project; }
        [DataMember] public string Name { get; private set; }
        [DataMember] public string Path { get; private set; }
    }
}
