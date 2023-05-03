using Editor.Controls;
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
using System.Security.Cryptography;
using System.Security.Policy;
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
                if (_name != value && _Validate(value))
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
                if (value == "")
                {
                    _procedure = null;
                    OnPropertyChanged(nameof(Name));
                }
                if (_procedure != value && _Validate(value))
                {
                    _procedure = value;
                    OnPropertyChanged(nameof(Name));
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
        

        private bool _Validate(string s)
        {
            bool result = true;
            if (s == null)
                result = false;
            if (s.Length == 0)
                result = false;
            if (s.Contains(" ") || s.Contains("*") || s.Contains("&"))
                result = false;

            return result;
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
            _windows.Add(new EngineWindow() { Height = 720, Width = 1280, Name = "g_mainWindow", IsFullScreen = false, Procedure = null, Title = "My window"});
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
            this.GetType();
            var scenes = Scenes.ToArray();
            foreach (var scene in scenes)
                RemoveScene(scene);
        }
        // SCENES
        public void AddScene(string sceneName)
        {
            Debug.Assert(!string.IsNullOrEmpty(sceneName.Trim()));
            _scenes.Add(new Scene(this) { Name = sceneName });
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
        private Engine.ScriptInfo[] _scripts = null;
        public Engine.ScriptInfo[] Scripts
        {
            get => _scripts;
            set
            {
                if (_scripts != value)
                {
                    _scripts = value;
                    OnPropertyChanged(nameof(Scripts));
                }
            }
        }
        private string GCDllPath { get => $@"{Path}{Name}\x64\Debug\GameAssembly.dll"; }
        public void CompileGameAssembly()
        {
            Engine.Scripting.UnloadGCDLL();
            SolutionManager.BuildProject(this, GameProjectType.GameAssembly, "Debug", new Action(() => {
                Engine.Scripting.LoadGCDLL(GCDllPath);
                Scripts = Engine.Scripting.GetScriptNames();
            }));
        }

        public void RunGameProject()
        {
            foreach(var scene in _scenes)
            {
                scene.SaveBin($@"{Path}{Name}\x64\Debug\{scene.Name.ToLower()}.bin");
            }
            SolutionManager.BuildProject(this, GameProjectType.Launcher, "Debug", new Action(() => {
                ProcessStartInfo process = new ProcessStartInfo();
                process.UseShellExecute = false;
                process.FileName = $@"{Path}{Name}\x64\Debug\{Name}.exe";
                using (System.Diagnostics.Process proc = System.Diagnostics.Process.Start(process))
                {
                    proc.WaitForExit();
                }
            }));
        }

        public void AddScript(string name)
        {
            var Cpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(Path, $"{Name}\\GameCode\\GameAssembly\\{name.ToLower()}.cpp"));
            var H = System.IO.Path.GetFullPath(System.IO.Path.Combine(Path, $"{Name}\\GameCode\\GameAssembly\\{name.ToLower()}.h"));
            var parameters = new Dictionary<string, object>()
            {
                {"ScriptName", name },
                {"Namespace", this.Name.ToLower() }
            };
            GameProject.ScriptHTemplate hTemplate = new GameProject.ScriptHTemplate();
            hTemplate.Session = parameters;
            hTemplate.Initialize();
            using (var sw = File.CreateText(H))
                sw.Write(hTemplate.TransformText());

            GameProject.ScriptCppTemplate cppTemplate = new GameProject.ScriptCppTemplate();
            cppTemplate.Session = parameters;
            cppTemplate.Initialize();
            using (var sw = File.CreateText(Cpp))
                sw.Write(cppTemplate.TransformText());

            GameProject.SolutionManager.AddFiles(System.IO.Path.GetFullPath(System.IO.Path.Combine(Path, $"{Name}.sln")), "GameAssembly", new string[] { Cpp, H });
        }

        public void UpdateGPFiles()
        {
            var parameters = new Dictionary<string, object>()
            {
                {"windows", this.Windows }
            };
            var projectPath = $@"{Path}{Name}\";
            var launcherMainCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\Launcher\\main.cpp"));
            GameProject.LauncherMainTemplate launcherTemplate = new GameProject.LauncherMainTemplate();
            launcherTemplate.Session = parameters;
            launcherTemplate.Initialize();
            using (var sw = File.CreateText(launcherMainCpp))
                sw.Write(launcherTemplate.TransformText());

            var assemblyMainH = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\GameAssembly\\assemblymain.h"));
            GameProject.AssemblyMainHTemplate assemblyMainHTemplate = new GameProject.AssemblyMainHTemplate();
            assemblyMainHTemplate.Session = parameters;
            assemblyMainHTemplate.Initialize();
            using (var sw = File.CreateText(assemblyMainH))
                sw.Write(assemblyMainHTemplate.TransformText());

            var assemblyMainCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(projectPath, $"GameCode\\GameAssembly\\assemblymain.cpp"));
            GameProject.AssemblyMainCPPTemplate assemblyMainCppTemplate = new GameProject.AssemblyMainCPPTemplate();
            assemblyMainCppTemplate.Session = parameters;
            assemblyMainCppTemplate.Initialize();
            using (var sw = File.CreateText(assemblyMainCpp))
                sw.Write(assemblyMainCppTemplate.TransformText());
        }

        // WINDOWS
        [DataMember] private ObservableCollection<EngineWindow> _windows = new ObservableCollection<EngineWindow>();
        public ReadOnlyObservableCollection<EngineWindow> Windows { get; set; }

        public void AddWindow(string name)
        {
            _windows.Add(new EngineWindow() { Title = name, Name = $"g_{name.ToLower()}", Height = 600, Width = 800, IsFullScreen = false, Procedure = "", ShowOnStartup = false });
        }

        public void AddWndProc(string procname)
        {
            GameProject.WndProcTemplate wndProcTemplate = new GameProject.WndProcTemplate();
            var wndProcCpp = System.IO.Path.GetFullPath(System.IO.Path.Combine(Path, $"{Name}\\GameCode\\GameAssembly\\{procname.ToLower()}.cpp"));
            var parameters = new Dictionary<string, object>()
            {
                {"procName", procname }
            };
            wndProcTemplate.Session = parameters;
            wndProcTemplate.Initialize();
            using (var sw = File.CreateText(wndProcCpp))
                sw.Write(wndProcTemplate.TransformText());
            GameProject.SolutionManager.AddFiles(System.IO.Path.GetFullPath(System.IO.Path.Combine(Path, $"{Name}.sln")), "GameAssembly", new string[] { wndProcCpp });
        }


        [DataMember(Name = "Scenes")] private ObservableCollection<Scene> _scenes = new ObservableCollection<Scene>();
        public ReadOnlyObservableCollection<Scene> Scenes { get; private set; }
        public static Project Current { get => Application.Current.MainWindow.DataContext as Project; }
        [DataMember] public string Name { get; private set; }
        [DataMember] public string Path { get; private set; }
        public string ContentPath { get => $@"{Path}{Name}\Content\"; }
    }
}
