using Editor.Utils;
using Engine;
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
using System.Xml.Linq;

namespace Editor.Project
{
    [DataContract]
    public class EngineWindow : VMBase
    {
        private string _name;
        [DataMember]
        public string Name
        {
            get => _name;
            set
            {
                if (_name != value)
                {
                    _name = value;
                    OnPropertyChanged(nameof(Name));
                }
            }
        }
        private string _title;
        [DataMember]
        public string Title
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

    [DataContract(Name = "Project")]
    public class Project : VMBase
    {
        // CTROR (ONLY FOR DEBUG PURPOSES) TODO
        public Project(string name)
        {
            Name = name;
            Path = "C:\\Users\\EXAMPLE_PATH\\REMOVE_LATER";
            Scenes = new ReadOnlyObservableCollection<Scene>(_scenes);
            Windows = new ReadOnlyObservableCollection<EngineWindow>(_windows);
        }
        [OnDeserialized]
        private void OnDeserialized(StreamingContext context)
        {
            Scenes = new ReadOnlyObservableCollection<Scene>(_scenes);
            Windows = new ReadOnlyObservableCollection<EngineWindow>(_windows);
        }
        // LOAD/UNLOAD
        public static Project Load(string file)
        {
            Debug.Assert(File.Exists(file));
            return Utils.Serializer.Deserialize<Project>(file);
        }
        public void Unload()
        {
            GameProject.SolutionManager.CloseVS();
        }
        // SCENES
        public void AddScene(string sceneName)
        {
            Debug.Assert(!string.IsNullOrEmpty(sceneName.Trim()));
            _scenes.Add(new Scene() { Name = sceneName });
        }
        public void RemoveScene(Scene scene)
        {
            Debug.Assert(_scenes.Contains(scene));
            scene.Destroy();
            _scenes.Remove(scene);
        }
        // WINDOWS
        [DataMember] private ObservableCollection<EngineWindow> _windows = new ObservableCollection<EngineWindow>();
        public ReadOnlyObservableCollection<EngineWindow> Windows;


        [DataMember(Name = "Scenes")] private ObservableCollection<Scene> _scenes = new ObservableCollection<Scene>();
        public ReadOnlyObservableCollection<Scene> Scenes { get; private set; }
        public static Project Current { get => Application.Current.MainWindow.DataContext as Project; }
        [DataMember] public string Name { get; private set; }
        [DataMember] public string Path { get; private set; }
    }
}
