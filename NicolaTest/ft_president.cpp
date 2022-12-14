#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <unistd.h>


template<class A, class B>
class eqClass
{
	public : 

		int				num;
		std::map<A, B>	list;
		std::set<A>	bordering;

		typedef typename std::map<A, B>::iterator	list_iterator;
		typedef typename std::map<A, B>::const_iterator	list_const_iterator;
		typedef typename std::set<A, B>::iterator	bordering_iterator;
		typedef typename std::set<A, B>::const_iterator	bordering_const_iterator;

		//Coplien
		eqClass() {}
		eqClass(const eqClass &other) : num(other.num), list(other.list), bordering(other.bordering)
		{}
		~eqClass() {}
		eqClass	&operator=(const eqClass &other)
		{
			num = other.num;
			list = other.list;
			bordering = other.bordering;
			return (*this);
		}
		//End Coplien

		//Equivalence class functions
		bool	findInBordering(const eqClass &other)	const
		{
			bordering_const_iterator	end = other.bordering.end();
			list_const_iterator endMap = list.end();
			for (bordering_const_iterator it = other.bordering.begin(); it != end; ++it)
			{
				if (list.find(*it) != endMap)
					return true;
			}
			return false;
		}

		void	unionEq(eqClass &other)
		{/*
			list_iterator endMap = other.list.end();
			for (list_iterator it1 = other.list.begin(); it1 != endMap; ++it1)
				bordering.erase(it1->first);

			bordering_iterator endSet = other.bordering.end();
			endMap = list.end();
			for (bordering_iterator it2 = other.bordering.begin(); it2 != endSet; ++it2)
			{
				if (list.find(*it2) == list.end())
				bordering.insert(*it2);
			}
			list.insert(other.list.begin(), other.list.end());*/
			list.insert(other.list.begin(), other.list.end());
			bordering.insert(other.bordering.begin(), other.bordering.end());
			std::set<std::string >::iterator end = bordering.end();
			std::set<std::string >::iterator it = bordering.begin();
			std::set<std::string >::iterator itNext;
			while (it != end)
			{
				itNext = it;
				++itNext;
				if (list.find(*it) != list.end())
					bordering.erase(it);
				it = itNext;
			}
		}

		int pib()	const
		{
			int sum = 0;
			list_const_iterator end = list.end();
			for (list_const_iterator it = list.begin(); it != end; ++it)
				sum += it->second;
			return (sum);
		}

		std::string display()	const
		{
			std::string	output;
			for (list_const_iterator it = list.begin(); it != list.end(); ++it)
			{
				output += it->first;
				if (it != --list.end())
					output += '-';
			}
			return (output);
		}

		std::set<std::string>	getSetList()	const
		{
			std::set<std::string>	setList;
			list_const_iterator end = list.end();
			for (list_const_iterator it = list.begin(); it != end; ++it)
				setList.insert(it->first);
			return setList;
		}
		//End Equivalence class functions
};

std::ostream	&operator<<(std::ostream &o, const std::map<std::string, int> &m)
{
	std::map<std::string, int>::const_iterator	end = m.end();
	for (std::map<std::string, int>::const_iterator it = m.begin(); it != end; ++it)
		o << "-> "<< it->first << " | " << it->second << '\n';
	return o;
}

std::ostream	&operator<<(std::ostream &o, const std::set<std::string> &s)
{
	std::set<std::string, int>::const_iterator	end = s.end();
	for (std::set<std::string, int>::const_iterator it = s.begin(); it != end; ++it)
		o << "-> " << *it << '\n';
	return o;
}

std::ostream	&operator<<(std::ostream &o, const eqClass<std::string, int> &eq)
{
	o << "----------eqClass " << eq.num << " ----------\n\n------ mapList ------\n" << eq.list << "\n---- setBordering ----\n" << eq.bordering;
	return o;
}

std::ostream	&operator<<(std::ostream &o, const std::vector<eqClass<std::string, int> > &v)
{
	for (size_t i = 0; i < v.size(); ++i)
		o << v[i] << '\n';
	return o;
}


class France
{
	public :

		typedef std::vector<eqClass<std::string, int> >		vecRegion;
		typedef std::vector<eqClass<std::string, int> >::iterator iterator;
		typedef std::vector<eqClass<std::string, int> >::const_iterator const_iterator;

		vecRegion	regions;

		France() {}
		France(const France &other) : regions(other.regions) {}
		France &operator=(const France &other)
		{
			regions = other.regions;
			return (*this);
		}
		~France() {}

		iterator begin() {return regions.begin();}
		const_iterator begin()	const {return regions.begin();}
		iterator end() {return regions.end();}
		const_iterator end()	const {return regions.end();}


		bool	parseEqClass(std::ifstream &input)
		{
			int	i = 1;
			std::string	line;
			while (!input.eof())
			{
				eqClass<std::string, int>	region;
				getline(input, line);
				if (line.back() == 13)	line.erase(--line.end());
				std::string::size_type	idxEndName = line.find(" : ", 0);
				std::string::size_type	idxBeginPib = idxEndName + 3;
				std::string::size_type	idxEndPib = line.find(" : ", idxBeginPib);
				std::string::size_type	idxBorder = idxEndPib + 3;
				if (idxEndName == std::string::npos || idxEndPib == std::string::npos || idxBorder >= line.size())
					return true;
				region.num = i++;
				region.list.insert(std::make_pair(line.substr(0, idxEndName), atoi(&line[idxBeginPib])));
				std::string::size_type	idxNextBorder = 0;
				while (idxNextBorder != std::string::npos)
				{
					idxNextBorder = line.find('-', idxBorder);
					region.bordering.insert(line.substr(idxBorder, idxNextBorder - idxBorder));
					idxBorder = idxNextBorder + 1;
				}
				regions.push_back(region);
			}
			return false;
		}

		double	variance()	const
		{
			double	average = 0;
			double	result = 0;
			int	sum = 0;
			std::vector<int>	pibVec;
			for (const_iterator it = regions.begin(); it != regions.end(); ++it) {
				sum += it->pib();
				pibVec.push_back(it->pib());
			}
			average = sum / pibVec.size();
			for (std::vector<int>::iterator it = pibVec.begin(); it != pibVec.end(); ++it)
				result += pow((static_cast<double>(*it) - average), 2);
			//result /= pibVec.size();
			return (result);
		}

		bool	tryToJoin(iterator it1, iterator it2)
		{
			if (!it1->findInBordering(*it2))
				return false;
			it1->unionEq(*it2);
			regions.erase(it2);
			return true;
		}

		void join(int idx1, int idx2)
        {
            iterator    it = regions.begin();
            while (&(*it) != &(regions[idx2]))
                ++it;
            regions[idx1].unionEq(regions[idx2]);
            regions.erase(it);
        }

		void displayResult(std::ostream &out)	const
		{
			for (const_iterator it = regions.begin(); it != regions.end(); ++it)
			{
				out << it->display();
				if (it + 1 != regions.end())
					out << '\n';
			}
		}

		std::set<std::set<std::string> >	getSetRegions()	const
		{
			std::set<std::set<std::string> >	setRegions;
			const_iterator end = regions.end();
			for (const_iterator it = regions.begin(); it != end; ++it)
				setRegions.insert (it->getSetList());
			return setRegions;
		}
};

std::ostream &operator<<(std::ostream &o, const France &f)
{
	o << "France has " << f.regions.size() << " regions which has pid's standard deviation at " << sqrt(f.variance() / f.regions.size()) << '\n';
	return o;
}

void	bruteFrance(const France &france, int nbRegionToJoin, std::pair<France, double> &bestStandDev, std::set<std::set<std::set<std::string> > > &known)
{
	if (!nbRegionToJoin)
	{
		double newStandDev = france.variance();
		//std::cout << newStandDev << '\n';
		if (newStandDev < bestStandDev.second)
		{
			bestStandDev.second = newStandDev;
			bestStandDev.first = france;
		}
	}
	else {
		for (France::const_iterator it1 = france.begin(); it1 != france.end() - 1; ++it1) {
			for (France::const_iterator it2 = it1 + 1; it2 != france.end(); ++it2) {
				if (it1->findInBordering(*it2))
				{
					std::pair<std::set<std::set<std::set<std::string> > >::iterator, bool> secure;
					int	idx1 = it1.base() - france.begin().base();
					int	idx2 = it2.base() - france.begin().base();
					France franceCpy(france);
					franceCpy.join(idx1, idx2);
					secure = known.insert(franceCpy.getSetRegions());
					if (secure.second)
						bruteFrance(franceCpy, nbRegionToJoin - 1, bestStandDev, known);
				}
			}
		}
	}
}

int	main(int ac, char **av)
{
	if (ac != 4)
		return (1);
	int	nbRegionNeeded = atoi(av[3]);

	std::ifstream	input1(av[1]);
	if (input1.fail())
		return (1);
	France	france;

	if (france.parseEqClass(input1))
	{
		std::cerr << "parsing failed\n";
		return 1;
	}

	//std::cout << france.regions << '\n';
	std::filebuf	fb;
	fb.open(av[2], std::ios::out);
	std::ostream	output(&fb);
	if (output.fail())
		return (1);

	if (!nbRegionNeeded)
	{
		output << "Error\n";
		return 0;
	}

	if (nbRegionNeeded >= france.regions.size()) {
		output << "Error\n";
		return 0;
	}
	std::pair<France, double>	result(france, 0xfffffffff);
	std::set<std::set<std::set<std::string> > >	known;
	//std::cout << france;
	bruteFrance(france, france.regions.size() - nbRegionNeeded, result, known);
	if (result.second < 0xfffffffff)
		result.first.displayResult(output);
	else
		output << "Error\n";
	std::cout << "final result = " << result.first;
	return 0;
}