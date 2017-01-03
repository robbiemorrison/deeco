<!--
  author  : Robbie Morrison
  date    : December 2016
  license : this work is licensed under a Creative Commons
            Attribution-ShareAlike 3.0 International License
-->

<div style="background: coral; padding: 10px; padding-left: 2em">
<b>This project is no longer active</b>.<br>
The source code and documentation are provided for historical reasons.<br>
The software was last compiled on 30&nbsp;September 2005.
</div>

<img src="img/gecko.png" width="200" style="float: right">

# *deeco*

*deeco* stands for dynamic energy, emissions, and cost optimization.

*deeco* is a high-resolution energy system modeling framework written in&nbsp;C++.  The software was developed in Germany between 1995 and 2005.

*deeco* captures technologies and costs, but not markets and prices.  A given model steps through a single year in hourly intervals, optimizing plant operations at each interval.  The software embeds the simplex linear programming algorithm from *Numerical Recipes in&nbsp;C*.   Individual models are specified using structured text files.  One of *deeco*'s strengths is its treatment of thermal processes.

Development became stranded when vendor support for a key programming library ceased.

The source code and documentation are provided on GitHub because they may be of interest to the energy modeling community.  In particular, some of the technology modules contain innovative designs.  Their class definitions can be inspected and their underlying theory is documented in a technical manual and elsewhere.

The source code is licensed under GNU&nbsp;GPLv2.

*deeco* is the subject of around 10&nbsp;refereed papers, 10&nbsp;conference presentations, and 10&nbsp;university theses.  A&nbsp;selection of this literature is given below.  Bibliographic information files in BibTeX, EndNote, and Wikipedia (WCF) formats can be found in the `refs` directory.

### Key resources

Web

* [Project website](http://www.iet.tu-berlin.de/deeco/) (official TU-Berlin version)
* [Project website](http://www.webcitation.org/query?url=http://www.iet.tu-berlin.de/deeco/&date=2016-12-20) (WebCite archive)
<!-- alternative URL: http://www.webcitation.org/6mtrnndbF -->

Technical manual

* [*Benutzerhandbuch* deeco — Version 1.0](docs/deeco-handbuch-10.pdf) (in German)

### Technology support

*deeco* supports the following types of technologies.

Demand plant

* outright electricity demand
* mechanical power demand
* space-heating demand

Conversion and transport plant

* standard boilers
* condensing boilers
* topping-cycle steam turbine plant
* gas-turbine cogeneration plant
* heat-pumps
* stationary fuel cells
* district heating grids
* waste-heat recovery

Sourcing plant

* electricity import/export
* chemical fuel import/export
* solar-thermal collectors
* photovoltaic cells
* wind-farms
* solar-thermal power plant

Storage plant

* daily, weekly, seasonal heat storage
* superconducting electricity storage

The various thermal processes within a *deeco* model negotiate amongst themselves as to who will set the flow and return temperatures (known as the thermal sub-network or TSN algorithm in some of the documentation).  The algorithm represents an attempt to mimic the behavior of real life control systems.

<img src="img/district-heating-02-458x291.png" width="458" style="float: none; border: 1px solid silver; margin-top: 1em;">

*One of the engineering configurations evaluated in Lindenburger et al (2000).*

### <a name="history"/>History

*deeco* was developed at the Institute for Theoretical Physics, University of Würzburg, Germany.  The first release was completed in late-1995.  Work later continued at the Institute for Energy Engineering (IET), Technical University of Berlin, Germany.  Software development ceased in September 2005 with version 006.2.  Use of the program ended in 2015.  The source code was published on GitHub on 3&nbsp;January 2017.

The original code was developed on an HP workstation and later ported to SCO UnixWare versions&nbsp;2 and&nbsp;7 running on a personal computer.  Details of the last development environment can be found in the [README.version](src/README.version) text file.

The codebase became stranded when Hewlett-Packard discontinued support for a key programming library and the UnixWare operating system was abandoned by The SCO Group.  The library was the AT&T Unix System Laboratories (USL) [Standard Components](http://osr600doc.xinuos.com/en/SDK_c++/CONTENTS.html) library (which predates the C++ Standard Template Library)

*deeco* was one of first energy system models to use hourly time steps.  By 2016, a decade later, the practice had become commonplace.

### Contributers

Thomas Bruckner (now University of Leipzig) wrote the original release.  Robbie Morrison ported *deeco* to SCO UnixWare, maintained the codebase, and wrote data processing scripts.  Dietmar Lindenberger, Johannes Bruhn, Kathrin Ramsel, Jan Heise, and Susanna Medel developed new technology modules and ran models.

### Printed documentation

* Bruckner, Thomas (19 December 1997). *deeco — Benutzerhandbuch* [*deeco — user guide*] (in German). Würzburg, Germany: Institut für Theoretische Physik der Universität Würzburg.

* Bruckner, Thomas (19 December 1997). *deeco — Programmer's manual*. Würzburg, Germany: Institute for Theoretical Physics, Würzburg University.

* Bruckner, Thomas (2001). [*Benutzerhandbuch deeco — Version&nbsp;1.0* [*User guide deeco — Version&nbsp;1.0*]](docs/deeco-handbuch-10.pdf) (in German). Berlin, Germany: Institut für Energietechnik, Technishe Universität Berlin.

### Selected publications

For background, a good place to start is Groscurth *et&nbsp;al* (1995) which describes the overarching structure of *deeco* and Bruckner *et&nbsp;al* (1997) which reports on its first application.  The most comprehensive description of *deeco* is given in the PhD by Thomas Bruckner (in German).

* Bruckner, Thomas (1997). [*Dynamische Energie- und Emissionsoptimierung regionaler Energiesysteme* [*Dynamic energy and emissions optimization for regional energy systems*]](docs/1997-bruckner-dynamische-energie-und-emissionsoptimierung-regionaler-energiesysteme-phd.pdf) (PhD). Würzburg, Germany: Institut für Theoretische Physik, Universität Würzburg.

* Groscurth, Helmuth-M, Thomas Bruckner, and Reiner Kümmel (September 1995). ["Modeling of energy-services supply systems"]( http://www.iet.tu-berlin.de/deeco/downloads/groscurth_etal_1995.pdf). *Energy*. **20** (9): 941–958. doi:[10.1016/0360-5442(95)00067-Q](https://dx.doi.org/10.1016/0360-5442(95)00067-Q). ISSN 0360-5442.

* Bruckner, Thomas, Helmuth-M Groscurth, and Reiner Kümmel (1997). ["Competition and synergy between energy technologies in municipal energy systems"](http://www.iet.tu-berlin.de/deeco/downloads/bruckner_etal_1997.pdf). *Energy*. **22** (10): 1005–1014. doi:[10.1016/S0360-5442(97)00037-6](https://dx.doi.org/10.1016/S0360-5442(97)00037-6). ISSN 0360-5442.

* Morrison, Robbie (2000). *[Optimizing exergy-services supply networks for sustainability](docs/2000-morrison-msc-bound.pdf)* (MSc). Dunedin, New Zealand: Physics Department, University of Otago.

* Lindenberger, Dietmar, Thomas Bruckner, Helmuth-M Groscurth, and Reiner Kümmel (2000). "Optimization of solar district heating systems: seasonal storage, heat pumps, and cogeneration". *Energy*. **25** (7): 591–608. doi:[10.1016/S0360-5442(99)00082-1](https://dx.doi.org/10.1016/S0360-5442(99)00082-1). ISSN 0360-5442.

* Morrison, Robbie and Thomas Bruckner (24–28 September 2002). Presented at Porto Venere, Italy. Ulgiati, Sergio, Mark T Brown, Mario Giampietro, Robert A Herendeen, and Kozo Mayumi (eds). ["High-resolution modeling of distributed energy resources using *deeco*: adverse interactions and potential policy conflicts"](http://www.wifa.uni-leipzig.de/fileadmin/user_upload/iirm-tm/energiemanagement/publikationen/Conference_Proceedings/2002_HighResModelDeeco.pdf). 3rd International Workshop: Advances in Energy Studies: Reconsidering the Importance of Energy. Padova, Italy: Servizi Grafici Editoriali. pp. 97–107.

* Bruckner, Thomas, Robbie Morrison, Chris Handley, and Murray Patterson (2003). ["High-resolution modeling of energy-services supply systems using *deeco*: overview and application to policy development"](http://www.wifa.uni-leipzig.de/fileadmin/user_upload/iirm-tm/energiemanagement/publikationen/reviewed_journals/2003_HighResolModelingEnergySystemsDeeco.pdf). *Annals of Operations Research* **121** (1–4): 151–180. doi:[10.1023/A:1023359303704](https://dx.doi.org/10.1023/A:1023359303704).

* Lindenberger, Dietmar, Thomas Bruckner, Robbie Morrison, Helmuth-M Groscurth, and Reiner Kümmel (2004). "Modernization of local energy systems". *Energy* **29** (2) 245–256. doi:[10.1016/S0360-5442(03)00063-X](https://dx.doi.org/10.1016/S0360-5442(03)00063-X). ISSN 0360-5442.

* Bruckner, Thomas, Robbie Morrison, and Tobias Wittmann (2005). ["Public policy modeling of distributed energy technologies: strategies, attributes, and challenges"](https://www.researchgate.net/profile/Thomas_Bruckner2/publication/222424177_Public_policy_modeling_of_distributed_energy_technologies_strategies_attributes_and_challenges/links/02bfe50e715d29e8e5000000.pdf). *Ecological Economics*. **54** (2–3): 328–245. doi:[10.1016/j.ecolecon.2004.12.032](https://dx.doi.org/10.1016/j.ecolecon.2004.12.032). ISSN 0921-8009.

### Contact

Robbie Morrison

<img src="img/emad.png" height="14" alt="." style="display: inline-block">

3&nbsp;January 2017

----

<a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/"><img alt="Creative Commons License" style="border-width: 0" src="https://i.creativecommons.org/l/by-sa/3.0/88x31.png"/></a><br/>This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/3.0/">Creative Commons Attribution-ShareAlike 3.0 International License</a>.

&#9634;

<!-- eof -->
