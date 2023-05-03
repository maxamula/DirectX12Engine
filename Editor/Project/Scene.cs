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
using System.Windows.Data;
using System.Windows.Markup;

namespace Editor.Project
{
    [DataContract(IsReference = true)]
    abstract public class SceneGraphBase : VMBase
    {
        abstract public GameObject CreateObject(string name);
        abstract public void Destroy();
        abstract public void _Detach(GameObject obj);
    }

    [DataContract]
    public class Scene : SceneGraphBase
    {
        // Engine side
        private Engine.Scene _engineScene;
        // Constructor
        public Scene(Project project)
        {
            _engineScene = new Engine.Scene();
            Objects = new ReadOnlyObservableCollection<GameObject>(_objects);
            Project = project;
        }

        [OnDeserialized]
        private void OnDeserialized(StreamingContext context)
        {
            _engineScene = new Engine.Scene();
            foreach(var child in _objects)
            {
                child._SetEngine(_engineScene.CreateObject());
            }
            Objects = new ReadOnlyObservableCollection<GameObject>(_objects);
        }

        public void SaveBin(string bin)
        {
            using (var bw = new BinaryWriter(File.Open(bin, FileMode.Open, FileAccess.Write)))
            {
                bw.Write(_objects.Count);
                foreach (var obj in _objects)
                {
                    bw.Write(0);    // reserved
                    bw.Write(obj.Components.Count);
                    foreach (var component in obj.Components)
                    {
                        bw.Write((int)component.GetComponentType());
                        component.WriteBin(bw);
                    }
                }
            }
        }
        public override GameObject CreateObject(string name)
        {
            // Creating new object
            var obj = new GameObject(this, this, _engineScene.CreateObject()) { Name = name };
            // Add object to scene list
            _objects.Add(obj);
            // Return object
            return obj;
        }

        public override void Destroy()
        {
            // destroy engine side and all children
            _engineScene.Destroy();
        }
        public override void _Detach(GameObject obj)
        {
            Debug.Assert(obj != null);
            _objects.Remove(obj);
        }
        [DataMember] public Project Project { get; private set; }
        [DataMember(Name = "Name")] private string _name;
        public string Name
        {
            get => _name;
            set
            {
                _name = value;
                OnPropertyChanged(nameof(Name));
            }
        }

        [DataMember] private ObservableCollection<GameObject> _objects = new ObservableCollection<GameObject>();
        public ReadOnlyObservableCollection<GameObject> Objects { get; private set; }
    }
}
