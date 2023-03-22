using Editor.Utils;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Project
{
    public abstract class WindowGraphNode : VMBase
    {
        protected string _title;
        virtual public string Title
        {
            get => _title;
            set
            {
                if (_title != value)
                {
                    _title = value;
                    OnPropertyChanged(nameof(Title));
                }
            }
        }
        public ObservableCollection<Connector> Input { get; set; } = new ObservableCollection<Connector>();
        public ObservableCollection<Connector> Output { get; set; } = new ObservableCollection<Connector>();
    }

    public class Connector : VMBase
    {
        protected string _title;
        virtual public string Title
        {
            get => _title;
            set
            {
                if (_title != value)
                {
                    _title = value;
                    OnPropertyChanged(nameof(Title));
                }
            }
        }
    }

    public class SceneNode : WindowGraphNode
    {
        public SceneNode()
        {
            Title = "Scene";
            Output.Add(new Connector() { Title = "Output" });
        }
    }

    public class WindowNode : WindowGraphNode
    {
        public WindowNode()
        {
            Title = "Window";
            Input.Add(new Connector() { Title = "Input" });
        }
        public override string Title
        {
            get => _title;
            set
            {
                _title = $"Window ({value})";
                OnPropertyChanged(nameof(Title));
            }
        }
    }

    public class WindowGraphEditor : VMBase
    {
        public ObservableCollection<WindowGraphNode> Nodes { get; } = new ObservableCollection<WindowGraphNode>();

        // TEMP
        public WindowGraphEditor()
        {
            Nodes.Add(new WindowNode());
            Nodes.Add(new WindowNode());
            Nodes.Add(new SceneNode());
        }
    }
}
