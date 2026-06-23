#pragma once


class MayaNode
{
public:
        MString                 mName;
        MMatrix                 mTransform;
        std::vector<MayaNode*>  mChilds;
        MayaModel*              mModel;
        MayaLight*              mLight;

        MayaNode() {
                mName = "Unnamed";
                mModel = 0;
                mLight = 0;
        }
};
