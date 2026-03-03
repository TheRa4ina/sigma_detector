#include <ryml.hpp>
#include <iostream>

void check_node_type(const ryml::NodeRef& node)
{
    if (node.is_map())
    {
        std::cout << "This is a MAP with " << node.num_children() << " key-value pairs\n";
    }
    else if (node.is_seq())
    {
        std::cout << "This is a SEQUENCE with " << node.num_children() << " items\n";
    }
    else if (node.is_val())
    {
        std::cout << "This is a SCALAR value: " << node.val() << "\n";
    }
    else if (node.is_stream() || node.is_doc())
    {
        std::cout << "This is a stream or doc node\n";
    }
    else
    {
        std::cout << "Unknown node type\n";
    }
}

int main()
{
    const char* yaml1 = R"(
selection_special:
    - Image:
        - ' /a'
        - ' /b'
    - Image1:
        - ' /c'
        - ' /d'
)";

    const char* yaml2 = R"(
selection_special:
    Image:
        - ' /a'
        - ' /b'
    Image1:
        - ' /c'
        - ' /d'
)";

    auto tree1 = ryml::parse_in_arena(yaml1);
    auto tree2 = ryml::parse_in_arena(yaml2);

    std::cout << "Case 1: ";
    check_node_type(tree1["selection_special"]);

    std::cout << "Case 2: ";
    check_node_type(tree2["selection_special"]);

    return 0;
}