using Microsoft.VisualBasic;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Content
{
    sealed class ContentInfo
    {
        public static int IconWidth => 90;
        public byte[] Icon { get; }
        public byte[] IconSmall { get; }
        public string FileName => Path.GetFileNameWithoutExtension(FullPath);
        public string FullPath { get; }
        public bool IsDirectory { get; }
        public DateTime LastModified { get; }
        public long? Size { get; }

        public ContentInfo(string fullpath, byte[] icon = null, byte[] iconsmall = null, DateTime? lastModified = null)
        {
            var info = new FileInfo(fullpath);
            IsDirectory = File.GetAttributes(fullpath).HasFlag(FileAttributes.Directory);
            LastModified = lastModified ?? info.LastWriteTime;
            Size = IsDirectory ? null : info.Length;
            Icon = icon;
            IconSmall = IconSmall ?? icon;
            FullPath = fullpath;
        }
    }

    public class ContentBrowser : VMBase
    {

    }
}
