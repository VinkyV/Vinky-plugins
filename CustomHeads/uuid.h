#include <random>

class random_char_source
{
    const std::string chars;
    std::mt19937 generator;
    std::uniform_int_distribution<std::size_t> distribution;

public:
    using iterator_category = std::input_iterator_tag;
    using value_type = char;
    using difference_type = int;
    using pointer = const char*;
    using reference = const char&;

    explicit random_char_source(std::string chars, std::random_device::result_type seed = std::random_device{}())
        : chars{ std::move(chars) },
        generator{ seed },
        distribution{ 0, this->chars.size() - 1 }
    {}

    // it's an input iterator
    char operator*() { return chars[distribution(generator)]; }
    random_char_source& operator++() { return *this; }
};

static std::string generate_random_id(std::size_t len)
{
    auto hex_chars = random_char_source{ "0123456789abcdef-" };

    std::string uuid;
    uuid.reserve(len);
    std::copy_n(hex_chars, len, std::back_inserter(uuid));
    return uuid;
}