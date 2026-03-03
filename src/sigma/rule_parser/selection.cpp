#include <sigma/rule_parser/selection.h>

namespace sigma {
    std::unique_ptr<ISelection> SelectionFactory::Create(std::string name, std::vector<std::unique_ptr<ISelectorBase>> selectors, bool isAll)
    {
        if (isAll)
            return std::make_unique<Selection<MatchesAll>>(std::move(name), std::move(selectors));
        else
            return std::make_unique<Selection<MatchesAny>>(std::move(name), std::move(selectors));
    }
}


