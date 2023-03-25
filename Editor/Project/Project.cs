using Editor.GameProject;
using Editor.Utils;
using Engine;
using EnvDTE;
using HandyControl.Controls;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
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
        [Category("Default properties")]
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
        [Category("Default properties")]
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
        private ushort _width;
        [DataMember]
        [Category("Default properties")]
        public ushort Width
        {
            get => _width;
            set
            {
                if (_width != value)
                {
                    _width = value;
                    OnPropertyChanged(nameof(Width));
                }
            }
        }
        private ushort _height;
        [DataMember]
        [Category("Default properties")]
        public ushort Height
        {
            get => _height;
            set
            {
                if (_height != value)
                {
                    _height = value;
                    OnPropertyChanged(nameof(Height));
                }
            }
        }
        private string _procedure;
        [DataMember]
        [Category("Default properties")]
        public string Procedure
        {
            get => _procedure;
            set
            {
                if (_procedure != value)
                {
                    _procedure = value;
                    OnPropertyChanged(nameof(Procedure));
                }
            }
        }
        private bool _isFullScreen;
        [DataMember]
        [Category("Default properties")]
        public bool IsFullScreen
        {
            get => _isFullScreen;
            set
            {
                if (_isFullScreen != value)
                {
                    _isFullScreen = value;
                    OnPropertyChanged(nameof(IsFullScreen));
                }
            }
        }
        private bool _showOnStartup;
        [DataMember]
        [Category("Default properties")]
        public bool ShowOnStartup
        {
            get => _showOnStartup;
            set
            {
                if (_showOnStartup != value)
                {
                    _showOnStartup = value;
                    OnPropertyChanged(nameof(ShowOnStartup));
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
            _windows.Add(new EngineWindow() { Height = 720, Width = 1280, Name = "g_mainWindow", IsFullScreen = false, Procedure = null, Title = "TITLE HERE"});
            _windows.Add(new EngineWindow() { Height = 720, Width = 1280, Name = "aah", IsFullScreen = false, Procedure = null, Title = "TITLE HERE"});
            _windows.Add(new EngineWindow() { Height = 720, Width = 1280, Name = "g_sideWindow", IsFullScreen = false, Procedure = null, Title = "TITLE HERE"});
            _windows.Add(new EngineWindow() { Height = 720, Width = 1280, Name = "g_camWindow", IsFullScreen = false, Procedure = null, Title = "TITLE HERE"});
        }
        [OnDeserialized]
        private void OnDeserialized(StreamingContext context)
        {
            Scenes = new ReadOnlyObservableCollection<Scene>(_scenes);
            Windows = new ReadOnlyObservableCollection<EngineWindow>(_windows);
        }
        // LOAD/UNLOAD
        public static void Save(Project project)
        {
            Debug.Assert(project != null);
            Utils.Serializer.Serialize<Project>(project, project.Path + $"{project.Name}\\{project.Name}.ahh");
            Growl.Success("Project saved successfully");
        }
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
        // SCRIPTING STUFF HERE
        private bool _isBuildAvailable = true;
        public bool IsBuildAvailable
        {
            get => _isBuildAvailable;
            set
            {
                if (_isBuildAvailable != value)
                {
                    _isBuildAvailable = value;
                    OnPropertyChanged(nameof(IsBuildAvailable));
                }
            }
        }
        private string GCDllPath { get => $@"{Path}{Name}\x64\Debug\GameAssembly.dll"; }
        public void BuildGameAssembly(Action callback = null)
        {
            SolutionManager.Build(this, "Debug", new Action(() =>
            {
                if (File.Exists(GCDllPath))
                {
                    Engine.Scripting.LoadGCDLL(GCDllPath);
                    //project.AvailableScripts = CLIEngine.Script.GetScriptNames();
                }
                else
                {
                    Debug.WriteLine($"GameAssembly.dll does not exist at {GCDllPath}");
                    Growl.Warning($"GameAssembly.dll does not exist at {GCDllPath}");
                }
            }), callback);
        }
        // WINDOWS
        [DataMember] private ObservableCollection<EngineWindow> _windows = new ObservableCollection<EngineWindow>();
        public ReadOnlyObservableCollection<EngineWindow> Windows { get; set; }


        [DataMember(Name = "Scenes")] private ObservableCollection<Scene> _scenes = new ObservableCollection<Scene>();
        public ReadOnlyObservableCollection<Scene> Scenes { get; private set; }
        public static Project Current { get => Application.Current.MainWindow.DataContext as Project; }
        [DataMember] public string Name { get; private set; }
        [DataMember] public string Path { get; private set; }
    }
}
