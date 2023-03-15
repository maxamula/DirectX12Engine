using Editor.Project;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Text;
using System.Threading.Tasks;


namespace Editor
{
    [DataContract(Name = "GObj")]
    public class GameObject : Project.SceneGraphBase
    {
        // Engine side
        private Engine.GameObject _engineObject;
        // CTROR
        public GameObject(Project.Scene scene, Project.SceneGraphBase parent, Engine.GameObject _engine) 
        {
            Debug.Assert(scene != null);
            _scene = scene;
            _parent = parent;
            _engineObject = _engine;
            Objects = new ReadOnlyObservableCollection<GameObject>(_objects);
        }
        // Manage graph
        public override GameObject CreateObject(string name)
        {
            // Create new obj
            var obj = new GameObject(_scene, this, _engineObject.CreateObject()) { Name = name };
            // Push into graph
            _objects.Add(obj);
            // Ret
            return obj;
        }

        public override void Destroy()
        {
            // Release objects engine side
            _engineObject.Destroy();    // This will release all engine-side child objects
            // Detach from parent
            _parent._Detach(this);
        }
        public override void _Detach(GameObject obj)
        {
            Debug.Assert(obj != null);
            // Remove from list
            _objects.Remove(obj);
        }

        // COMPONENTS MANAGEMENT

        private Project.SceneGraphBase _parent;
        private Project.Scene _scene;
        public Project.Scene ParentScene
        {
            get => _scene;
            private set
            {
                if(_scene != value)
                {
                    _scene = value;
                    OnPropertyChanged(nameof(ParentScene));
                }
            }
        }

        private ObservableCollection<GameObject> _objects = new ObservableCollection<GameObject>();
        public ReadOnlyObservableCollection<GameObject> Objects { get; private set; }

        // MISC
        private string _name;
        public string Name
        {
            get => _name;
            set
            {
                if(value != _name) 
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
    }
}
