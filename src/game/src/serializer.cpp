#include <game/serializer.h>

#include <filesystem>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <game/config.h>
#include <game/log.h>
#include <game/version/version.h>


using Node = boost::property_tree::ptree;


namespace
{

const std::filesystem::path CONFIG_FILE_NAME = "config.xml";
const std::string ATTRIBUTE_VERSION = "Version";
const std::string TAG_CONFIG = "Config";
const std::string TAG_CRANES_QUANTITY = "CranesQuantity";
const std::string TAG_SOUND = "Sound";
const std::string TAG_HIGH_SCORE = "HighScore";


bool writeFile(
    const std::filesystem::path &fileName,
    const Node &tree)
{
    namespace pt = boost::property_tree;

    const pt::xml_writer_settings<Node::key_type> indent =
        pt::xml_writer_make_settings<Node::key_type>(' ', 2);

    try
    {
        pt::write_xml(fileName.string(), tree, std::locale(), indent);
    }
    catch (const pt::xml_parser_error &error)
    {
        LOG_ERROR("Failed to write configuration: " << error.message());
        return false;
    }

    LOG_INFO("Configuration has been written.");
    return true;
}

void parseNodeConfig(
    const boost::property_tree::ptree::value_type &node,
    Config &config)
{
    namespace pt = boost::property_tree;
    pt::ptree tree;
    for (const pt::ptree::value_type &subNode : node.second)
    {
        if (subNode.first == TAG_CRANES_QUANTITY)
        {
            config.cranesQuantity = subNode.second.get<std::uint8_t>("");
            config.cranesQuantity = std::clamp(
                config.cranesQuantity,
                std::uint8_t(1),
                MAX_INITIAL_CRANES_QUANTITY);
        }
        if (subNode.first == TAG_SOUND)
        {
            config.sound = subNode.second.get<bool>("");
        }
        if (subNode.first == TAG_HIGH_SCORE)
        {
            config.highScore = subNode.second.get<unsigned int>("");
        }
    }
}

void parseFile(
    const std::filesystem::path &path,
    Config &config)
{
    namespace pt = boost::property_tree;
    pt::ptree tree;

    pt::read_xml(path.string(), tree, pt::xml_parser::no_comments);
    for (const pt::ptree::value_type &node : tree)
    {
        if (node.first != ProjectName)
        {
            continue;
        }

        LOG_INFO(
            "Configuration version: "
            << node.second.get("<xmlattr>." + ATTRIBUTE_VERSION, "Unknown version")
            << '.');

        for (const pt::ptree::value_type &nodeConfig : node.second)
        {
            if (nodeConfig.first != TAG_CONFIG)
            {
                continue;
            }
            parseNodeConfig(nodeConfig, config);
        }
    }
}

}


bool writeConfig()
{
    const std::filesystem::path &path = CONFIG_FILE_NAME;
    LOG_INFO("Writing configuration into " << path << ".");

    const Config &config = Config::instance();

    Node tree;

    Node &nodeRoot = tree.add_child(ProjectName, Node());
    nodeRoot.put("<xmlattr>." + ATTRIBUTE_VERSION, ProjectVersionMajorMinor);

    Node &nodeConfig = nodeRoot.add_child(TAG_CONFIG, Node());
    nodeConfig.add(TAG_CRANES_QUANTITY, config.cranesQuantity);
    nodeConfig.add(TAG_SOUND, config.sound);
    nodeConfig.add(TAG_HIGH_SCORE, config.highScore);

    return writeFile(path, tree);
}


bool readConfig()
{
    const std::filesystem::path &path = CONFIG_FILE_NAME;
    LOG_INFO("Reading configuration from " << path << ".");

    Config &config = Config::instance();
    try
    {
        parseFile(path, config);
    }
    catch (const std::exception &exception)
    {
        LOG_ERROR("Failed to read configuration: " << exception.what() << ".")
        return false;
    }

    LOG_INFO(
        "Configuration has been read:" << std::endl
        << "  cranes quantity: " << unsigned(config.cranesQuantity) << std::endl
        << "  sound: " << config.sound << std::endl
        << "  highScore: " << config.highScore);

    return true;
}
