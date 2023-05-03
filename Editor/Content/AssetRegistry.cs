using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Timers;
using System.Windows;

namespace Editor.Content
{
    public class FileEventFilter
    {
        private static Dictionary<Timer, string> s_fileEventFilters = new Dictionary<Timer,string>();
        public static bool CanFireEvent(FileSystemEventArgs e) => !s_fileEventFilters.ContainsValue(e.FullPath);
        public static void AddDelay(FileSystemEventArgs e)
        {
            Timer timer = new Timer(70);
            timer.Elapsed += (sender, e) => s_fileEventFilters.Remove(sender as Timer);
            s_fileEventFilters.Add(timer, e.FullPath);
            timer.Start();
        }
    }
    static class AssetRegistry
    {
        private static readonly Dictionary<string, AssetInfo> s_assetDictionary = new Dictionary<string, AssetInfo>();
        private static readonly ObservableCollection<AssetInfo> s_assets = new ObservableCollection<AssetInfo>();
        private static readonly FileSystemWatcher s_watcher = new FileSystemWatcher() { IncludeSubdirectories = true, Filter="", NotifyFilter = NotifyFilters.CreationTime | NotifyFilters.DirectoryName | NotifyFilters.FileName | NotifyFilters.LastWrite };
        
        public static ReadOnlyObservableCollection<AssetInfo> Assets { get; } = new ReadOnlyObservableCollection<AssetInfo>(s_assets);

        static AssetRegistry()
        {
            s_watcher.Changed += OnContentModified;
            s_watcher.Created += OnContentModified;
            s_watcher.Deleted += OnContentModified;
            s_watcher.Renamed += OnContentModified;
        }

        private static async void OnContentModified(object sender, FileSystemEventArgs e)
        {
            if(FileEventFilter.CanFireEvent(e))
            {
                FileEventFilter.AddDelay(e);
                if (Path.GetExtension(e.FullPath) != ".asset") return;
            }
        }

        public static void Reset(string contentFolder)
        {
            s_watcher.EnableRaisingEvents = false;
            s_assetDictionary.Clear();
            s_assets.Clear();
            Debug.Assert(Directory.Exists(contentFolder));
            RegisterAllAssets(contentFolder);
            s_watcher.Path = contentFolder;
            s_watcher.EnableRaisingEvents = true;
        }

        private static bool IsOlder(this DateTime date, DateTime other) => date < other;
        private static void RegisterAllAssets(string path)
        {
            foreach (var entry in Directory.GetFileSystemEntries(path))
            {
                if (Directory.Exists(entry) && File.GetAttributes(entry).HasFlag(FileAttributes.Directory))
                {
                    RegisterAllAssets(entry);
                }
                else
                {
                    var info = Asset.GetAssetInfo(entry);
                    if (!s_assetDictionary.ContainsKey(path) || info.ImportDate.IsOlder(s_assetDictionary[path].ImportDate))
                    {
                        s_assetDictionary.Add(info.FullPath, info);
                    }
                    s_assets.Add(info);
                }
            }
        }
    }
}
