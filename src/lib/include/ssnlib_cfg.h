

/*-
 * MIT License
 *
 * Copyright (c) 2017 Susanoo G
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 * @file ssnlib_cfg.h
 * @brief wrap rte_cfgfile.h
 * @author slankdev
 */

#pragma once
#include <rte_cfgfile.h>


namespace ssnlib {


struct Entry {
    std::string name;
    std::string value;
};

struct Section {
    std::string name;
    std::vector<Entry> entries;
};


class Config {
    rte_cfgfile* cfg;
public:
    std::vector<Section> sections;

    Config() : cfg(nullptr) {}
    ~Config() { close(); }
    void load(const char* filename)
    {
        if (cfg) throw slankdev::exception("already load config");

        cfg = rte_cfgfile_load(filename, 0);
        if (!cfg)
            throw slankdev::exception("rte_cfgfile_load");

        char **section_names;
        const size_t nb_sections = rte_cfgfile_num_sections(cfg, nullptr, 0);
        sections.resize(nb_sections);

        section_names = (char**)malloc(nb_sections * sizeof(char*));
        for (size_t i = 0; i < nb_sections; i++)
            section_names[i] = (char*)malloc(CFG_NAME_LEN);

        int ret = rte_cfgfile_sections(cfg, section_names, nb_sections);
        if (ret < 0) throw slankdev::exception("rte_cfgfile_sections");

        for (size_t i = 0; i < nb_sections; i++) {
            sections[i].name = section_names[i];
            const size_t nb_entries = rte_cfgfile_section_num_entries(cfg, section_names[i]);
            rte_cfgfile_entry* ents = (rte_cfgfile_entry*)malloc(nb_entries * sizeof(rte_cfgfile_entry));
            ret = rte_cfgfile_section_entries(cfg, section_names[i], ents, nb_entries);
            sections[i].entries.resize(nb_entries);
            if (ret < 0) throw slankdev::exception("rte_cfgfile_sections");
            for (size_t j=0; j<nb_entries; j++) {
                sections[i].entries[j].name  = ents[j].name;
                sections[i].entries[j].value = ents[j].value;
            }
            free(ents);
        }

        for (size_t i = 0; i < nb_sections; i++)
            free(section_names[i]);
    }
    bool has_section(const char* sec_name)
    {
        int ret = rte_cfgfile_has_section(cfg, sec_name);
        return ret != 0;
    }
    bool has_entry(const char* sec_name, const char* ent_name)
    {
        int ret = rte_cfgfile_has_entry(cfg, sec_name, ent_name);
        return ret != 0;
    }
    std::string get_entry(const char* sec_name, const char* ent_name)
    {
        const char* str = rte_cfgfile_get_entry(cfg, sec_name, ent_name);
        if (!str) throw slankdev::exception("rte_cfgfile_get_entry");
        return str;

    }
    void close()
    {
        if (cfg) {
            rte_cfgfile_close(cfg);
            cfg = nullptr;
        }
    }
};



} /* namespace ssnlib */
