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

namespace Editor.Project
{
    [DataContract(Name = "Project")]
    public class Project : VMBase
    {
        // CTROR (ONLY FOR DEBUG PURPOSES)
        public Project(string name) 
        {
            Name = name;
            Path = "C:\\Users\\EXAMPLE_PATH\\REMOVE_LATER";
            Scenes = new ReadOnlyObservableCollection<Scene>(_scenes);
        }
        // LOAD/UNLOAD
        public static Project Load(string file)
        {
            Debug.Assert(File.Exists(file));
            return Utils.Serializer.Deserialize<Project>(file);
        }
        public void Unload()
        {
            // TODO close vs solution
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
        [DataMember(Name = "Scenes")] private ObservableCollection<Scene> _scenes = new ObservableCollection<Scene>();
        public ReadOnlyObservableCollection<Scene> Scenes { get; private set; }
        public static Project Current { get => Application.Current.MainWindow.DataContext as Project; }
        [DataMember] public string Name { get; private set; }
        [DataMember] public string Path { get; private set; }
    }
}
