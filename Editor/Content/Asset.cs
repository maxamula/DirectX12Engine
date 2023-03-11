using Editor.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Editor.Content
{
    public enum AssetType
    {
        Unknown,
        Animation,
        Audio,
        Material,
        Mesh,
        Sleleton,
        Texture,
    }

    abstract public class Asset : VMBase
    {
        public Asset(AssetType assetType)
        {
            Debug.Assert(assetType != AssetType.Unknown);
            Type = assetType;
        }

        public AssetType Type { get; private set; }
    }
}
