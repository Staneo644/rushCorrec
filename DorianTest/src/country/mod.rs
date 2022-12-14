mod organize;
mod region;
use region::{Money, Region};

use rayon::prelude::*;
use std::cmp::Ordering;
use std::collections::HashMap;
use std::fmt::{self, Formatter};
use std::str::FromStr;
use std::sync::RwLock;

fn money_compare(a: &Money, b: &Money) -> Ordering {
    a.partial_cmp(&b).unwrap_or(Ordering::Less)
}
fn region_compare(a: &&Region, b: &&Region) -> Ordering {
    money_compare(&a.gdp, &b.gdp)
}

#[derive(Debug, Clone)]
pub struct Country {
    pub regions: HashMap<String, Region>,
}

impl Country {
    /// Calculates the total GDP.
    fn total_gdp(&self) -> Money {
        self.regions.values().map(|r| r.gdp).sum()
    }

    /// Calculated the GDP average.
    fn avg_gdp(&self) -> Money {
        self.total_gdp() / self.regions.len() as Money
    }

    /// Calculates the GDP standard deviation squared.
    /// This is useful when comparing GDPs.
    /// As square roots can be computationally heavy.
    fn std_dev_sq(&self) -> Money {
        let avg_gdp = self.avg_gdp();

        self.regions
            .values()
            .map(|r| (r.gdp - avg_gdp).powi(2))
            .sum::<Money>()
            / self.regions.len() as Money
    }

    /// Calculates the GDP standard deviation.
    fn std_dev(&self) -> Money {
        self.std_dev_sq().sqrt()
    }

    /// Generates a minimal accessible bound
    /// by the GDP's standard deviation by region fusion.
    fn optimal_std_dev_sq(&self, target_count: usize) -> Money {
        let mut gdp_sorted: Vec<_> = self.regions.values().map(|r| r.gdp).collect();
        gdp_sorted.sort_by(money_compare);
        let to_fuse = gdp_sorted.len() - target_count;
        let to_spread: Money = gdp_sorted[..to_fuse].iter().sum();
        let to_spread_on = &gdp_sorted[to_fuse..];
        let target_avg = (to_spread + to_spread_on.iter().sum::<Money>()) / target_count as Money;
        let mut sum = 0.0;
        for (i, (a, b)) in to_spread_on
            .iter()
            .zip(to_spread_on.iter().skip(1))
            .enumerate()
        {
            let width = i + 1;
            sum += a;
            if sum + to_spread < b * (width + 1) as Money {
                let spread_gdp = (sum + to_spread) / width as Money;
                return ((target_avg - spread_gdp).powi(2) * (width as Money)
                    + to_spread_on[width..]
                        .iter()
                        .map(|gdp| (target_avg - gdp).powi(2))
                        .sum::<Money>())
                    / target_count as Money;
            }
        }
        0.0
    }

    /// Optimize regions by fusing them in order to
    /// reduce the GDP's standard deviation.
    /// Resulting in a country with <target_count> regions.
    pub fn optimize(&mut self, target_count: usize) -> Result<(), ()> {
        match target_count.cmp(&self.regions.len()) {
            Ordering::Equal => return Ok(()),
            Ordering::Greater => return Err(()),
            _ => {}
        }
        if target_count >= self.regions.len() {
            return Err(());
        }
        let mut links: Vec<(String, String)> = self
            .regions
            .values()
            .map(|r| r.links.iter().map(|l| (r.name.clone(), l.clone())))
            .flatten()
            .collect();
        links.sort();
        links.dedup();
        let best = links
            .into_par_iter()
            .map(|link| {
                let mut cloned = self.clone();
                cloned.fuse_regions((link.0.as_ref(), link.1.as_ref()));
                // TODO change
                cloned.optimize(target_count).unwrap();
                cloned
            })
            .min_by(|a, b| {
                a.std_dev_sq()
                    .partial_cmp(&b.std_dev_sq())
                    .unwrap_or(Ordering::Less)
            });
        best.map(|b| *self = b).ok_or(())
    }

    pub fn optimize2(&mut self, target_count: usize) -> Result<(), ()> {
        match target_count.cmp(&self.regions.len()) {
            Ordering::Equal => return Ok(()),
            Ordering::Greater => return Err(()),
            _ => {}
        }

        //        let mut links: Vec<(String, String)> = self
        //        .regions
        //        .values()
        //        .map(|r| r.links.iter().map(|l| if &r.name < l { (r.name.clone(), l.clone()) } else { (l.clone(), r.name.clone()) }))
        //        .flatten()
        //        .collect();

        let mut links = vec![];

        for region in self.regions.values() {
            for link in region.links.iter() {
                if &region.name > link {
                    links.push((region.name.clone(), link.clone()));
                }
            }
        }

        let best = links
            .into_par_iter()
            .map(|link| {
                let mut cloned = self.clone();
                cloned.fuse_regions((link.0.as_ref(), link.1.as_ref()));
                // TODO change
                cloned.optimize2(target_count).unwrap();
                cloned
            })
            .min_by(|a, b| {
                a.std_dev_sq()
                    .partial_cmp(&b.std_dev_sq())
                    .unwrap_or(Ordering::Less)
            });
        best.map(|b| *self = b).ok_or(())
    }

    pub fn optimize3(&mut self, target_count: usize) -> Result<(), &'static str> {
        match target_count.cmp(&self.regions.len()) {
            Ordering::Equal => return Ok(()),
            Ordering::Greater => {
                return Err("target region count cannot be less than initial region count")
            }
            _ => {}
        }
        // let mut best_yet = Money::INFINITY;
        let mut best_yet = RwLock::new((Money::INFINITY, None));
        self.optimize3_recursion(target_count, &mut best_yet);
        best_yet
            .into_inner()
            .expect("Could not recover best result")
            .1
            .map(|r| *self = r)
            .ok_or("Could not find an optimal solution")
    }

    pub fn optimize3_recursion(
        &self,
        target_count: usize,
        best_yet: &RwLock<(Money, Option<Country>)>,
    ) {
        if target_count == self.regions.len() {
            let std_dev_sq = self.std_dev_sq();
            let mut writer = best_yet.write().unwrap();
            if std_dev_sq < writer.0 {
                *writer = (std_dev_sq, Some(self.clone()));
            }
            return;
        }

        // TODO: refactor into iterator
        let mut regions_sorted: Vec<_> = self.regions.values().collect();
        regions_sorted.sort_by(region_compare);
        let mut links = vec![];
        for region in regions_sorted {
            for link in region.links.iter() {
                if &region.name > link {
                    links.push((region.name.clone(), link.clone()));
                }
            }
        }

        // TODO: find optimal euristic
        // links.sort_by(|a, b| {
        //     (self.regions[&a.0].gdp + self.regions[&a.1].gdp).partial_cmp(
        //         &(self.regions[&b.0].gdp + self.regions[&b.1].gdp)
        //     ).unwrap_or(Ordering::Less)
        // });

        // let mut best = None;
        // for link in links {
        //     let mut cloned = self.clone();
        //     cloned.fuse_regions((link.0.as_ref(), link.1.as_ref()));
        //     if cloned.optimal_std_dev_sq2(target_count) < *best_yet {
        //         let result = cloned.optimize3_quel_enfer(target_count, best_yet);
        //         if result.is_some() {
        //             best = result;
        //         }
        //     }
        // }
        links
            .par_iter()
            .map(|link| {
                let mut cloned = self.clone();
                cloned.fuse_regions((link.0.as_ref(), link.1.as_ref()));
                if cloned.optimal_std_dev_sq(target_count) < best_yet.read().unwrap().0 {
                    cloned.optimize3_recursion(target_count, best_yet);
                }
            })
            .for_each(|_| {});
    }

    fn remove_link_from_region(&mut self, region_name: &str, name: &str) {
        let pos = self.regions[region_name]
            .links
            .iter()
            .position(|r| r == &name);

        pos.map(|p| self.regions.get_mut(region_name).unwrap().links.remove(p));
    }

    fn add_link_to_region(&mut self, region_name: &str, name: &str) {
        self.regions
            .get_mut(region_name)
            .unwrap()
            .links
            .push(name.into());
    }

    fn fuse_regions(&mut self, (left_name, right_name): (&str, &str)) {
        let left = self.regions.remove(left_name).unwrap();
        let right = self.regions.remove(right_name).unwrap();
        let fused = left.fuse(right);

        for region_name in &fused.links {
            self.remove_link_from_region(region_name, left_name);
            self.remove_link_from_region(region_name, right_name);

            self.add_link_to_region(region_name, &fused.name);
        }

        self.regions.insert(fused.name.clone(), fused);
    }
}

impl FromStr for Country {
    type Err = <Region as FromStr>::Err;

    fn from_str(input: &str) -> Result<Self, Self::Err> {
        let regions = input
            .lines()
            .map(|l| l.parse().map(|r: Region| (r.name.clone(), r)))
            .collect::<Result<_, _>>()?;

        Ok(Self { regions })
    }
}

impl fmt::Display for Country {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        writeln!(f, "The avg GDP is {}", self.avg_gdp())?;
        writeln!(f, "The std_dev_sq is {}", self.std_dev_sq())?;
        writeln!(f, "The std_dev is {}", self.std_dev())
    }
}

#[cfg(test)]
mod tests {
    use crate::Country;

    const INPUT: &str = include_str!("../../subject/exempleRegion.txt");
    const INPUT_TEST: &str = include_str!("../../subject/exempleTest.txt");

    #[test]
    fn remove_link_from_region() {
        let mut country: Country = INPUT.parse().unwrap();

        country.remove_link_from_region("Nord", "Paris");
        country.remove_link_from_region("Nord", "Normandie");

        assert!(!country.regions["Nord"].links.contains(&"Paris".into()));
        assert!(!country.regions["Nord"].links.contains(&"Normandie".into()));
    }

    #[test]
    fn add_link_to_region() {
        let mut country: Country = INPUT.parse().unwrap();

        country.add_link_to_region("Nord", "Nouvelle-Acquitaine");

        assert!(country.regions["Nord"]
            .links
            .contains(&"Nouvelle-Acquitaine".into()));
    }

    #[test]
    fn region_fuse() {
        let mut country: Country = INPUT_TEST.parse().unwrap();

        country.fuse_regions(("A", "C"));

        assert_eq!(country.regions["A-C"].links, vec!["B", "D"]);
        assert_eq!(country.regions["B"].links, vec!["A-C"]);
        assert_eq!(country.regions["D"].links, vec!["A-C"]);
    }

    fn check_bidir_links(country: Country) {
        for region in country.regions.values() {
            for other in region.links.iter() {
                assert!(country.regions[other].links.contains(&region.name));
            }
        }
    }

    #[test]
    fn bidirectional_links() {
        check_bidir_links(INPUT.parse().unwrap());
        check_bidir_links(INPUT_TEST.parse().unwrap());
    }

    #[test]
    fn optimal_std_dev_sq() {
        let country: Country = INPUT_TEST.parse().unwrap();

        assert_eq!(country.optimal_std_dev_sq(2), 1.0)
    }

    #[test]
    fn optimal_std_dev_sq2() {
        let country: Country = INPUT_TEST.parse().unwrap();
        assert_eq!(country.optimal_std_dev_sq(2), 1.0);
    }

    #[test]
    fn optmize_with_zero_size() {
        let mut country: Country = INPUT_TEST.parse().unwrap();
        assert!(
            matches!(country.optimize3(0), Err(_)),
            "Optmizing down to 0 elements should not be possible"
        )
    }
}
