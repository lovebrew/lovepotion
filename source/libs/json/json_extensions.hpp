// libstarlight extensions for nlohmann::json (include nested within basic_json)

// try_get: reads out if applicable, else leaves the target alone
void try_get(int& v) { if (is_number()) v = get<int>(); }
void try_get(float& v) { if (is_number()) v = get<float>(); }
void try_get(std::string& v) { if (is_string()) v = get<std::string>(); }

/*bool has(std::string& idx) const {
    return find(idx) != end();
}*/
