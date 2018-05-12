#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cassert>
#include <map>
#include <set>
#include <sstream>
#include <string>

//@class options
class options
{
public:
	//@class def
	class def
	{
	public:
		def(const std::string& name,
		    const std::string& defaultValue)
			: m_name(name)
			, m_value(defaultValue)
		{
		}

		//@brief gets option name
		const std::string& getName() const
		{
			return m_name;
		}

		//@brief gets option value
		const std::string& getValue() const
		{
			return m_value;
		}

		//@brief set option value
		void setValue(const std::string& v)
		{
			m_value = v;
		}
	public:
		std::string m_name;
		std::string m_value;
	}; // class def

private:
	typedef std::map<std::string, def> nameToDef;
public:
	//@brief adds definition of option
	void addOption(const def& d)
	{
		m_nameToDef.insert(std::make_pair(d.getName(), d));
	}

	//@brief checks the given option anme
	bool isValidOption(const std::string& name) const
	{
		return m_nameToDef.find(name) != m_nameToDef.end();
	}

	//@brief sets value for given option
	void setValue(const std::string& name, const std::string& value)
	{
		assert(isValidOption(name));
		nameToDef::iterator i = m_nameToDef.find(name);
		i->second.setValue(value);
	}

	//@brief gets value for given option
	template <typename ValueType>
	bool getValue(const std::string& name, ValueType& value) const
	{
		assert(isValidOption(name));
		nameToDef::const_iterator i = m_nameToDef.find(name);
		std::stringstream ss(i->second.getValue());
		ss >> value;
		return !ss.fail();
	}
private:
	nameToDef m_nameToDef;
}; // class options 

#endif // OPTIONS_HPP
