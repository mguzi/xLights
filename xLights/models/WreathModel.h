#ifndef WREATHMODEL_H
#define WREATHMODEL_H

#include "Model.h"


class WreathModel : public Model
{
    public:
        WreathModel(wxXmlNode *node, const NetInfoClass &netInfo, bool zeroBased = false);
        virtual ~WreathModel();

    protected:
        virtual void InitModel() override;

    private:
        void InitWreath();
};

#endif // WREATHMODEL_H