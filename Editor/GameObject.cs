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
using System.Windows;
using System.Xml.Linq;
using Editor.Components;

[assembly: InternalsVisibleTo("Editor.Project")]
[assembly: InternalsVisibleTo("Editor.Components")]
namespace Editor
{
    [DataContract(Name = "Object")]
    public class GameObject : Project.SceneGraphBase
    {
        // Engine side
        internal Engine.GameObject _engineObject;
        // CTROR
        public GameObject(Project.Scene scene, Project.SceneGraphBase parent, Engine.GameObject _engine)
        {
            Debug.Assert(scene != null);
            _scene = scene;
            _parent = parent;
            _engineObject = _engine;
            _components.Add(new Transformation(this));
            Objects = new ReadOnlyObservableCollection<GameObject>(_objects);
            Components = new ReadOnlyObservableCollection<Component>(_components);
        }
        [OnDeserialized]
        private void OnDeserialized(StreamingContext context)
        {
            Components = new ReadOnlyObservableCollection<Component>(_components);
            Objects = new ReadOnlyObservableCollection<GameObject>(_objects);
        }
        internal void _SetEngine(Engine.GameObject o)
        {
            // Set engine side object
            _engineObject = o;
            // Apply components to engine
            foreach(var c in _components)
            {
                c.ApplyToEngine();
            }
            // Recursively set for children
            foreach(var child in _objects)
            {
                child._SetEngine(_engineObject.CreateObject());
            }

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
        public void AddComponent(Component component)
        {
            Debug.Assert(component != null);
            // Check if there is already component of this type
            foreach(var c in _components)
            {
                if(c.GetType() == component.GetType())
                {
                    MessageBox.Show("There is already component of this type");
                    return;
                }
            }
            _components.Add(component);
        }

        public void RemoveComponent(Component component)
        {
            Debug.Assert(_components.Contains(component));
            if(!(component is int))
            {
                _components.Remove(component);
            }
        }

        public T GetComponent<T>() where T : Component
        {
            return _components.FirstOrDefault(c => c is T) as T;
        }

        [DataMember] private Project.SceneGraphBase _parent;
        [DataMember] private Project.Scene _scene;
        public Project.Scene ParentScene
        {
            get => _scene;
            private set
            {
                if (_scene != value)
                {
                    _scene = value;
                    OnPropertyChanged(nameof(ParentScene));
                }
            }
        }

        [DataMember]
        private ObservableCollection<Component> _components = new ObservableCollection<Component>();
        public ReadOnlyObservableCollection<Component> Components { get; private set; }

        [DataMember] private ObservableCollection<GameObject> _objects = new ObservableCollection<GameObject>();
        public ReadOnlyObservableCollection<GameObject> Objects { get; private set; }

        // MISC
        private string _name;
        [DataMember] public string Name
        {
            get => _name;
            set
            {
                if (value != _name)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
    }
}
