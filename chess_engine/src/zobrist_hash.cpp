#include "zobrist_hash.h"
#include "board.h"

// https://www.chessprogramming.org/Zobrist_Hashing

namespace
{

constexpr int c_piece_count{6};
constexpr int c_pieces_region_size{c_board_dimension_squared * c_piece_count};
constexpr int c_black_pieces_offset{0};
constexpr int c_white_pieces_offset{c_black_pieces_offset + c_pieces_region_size};

constexpr int c_black_to_move_offset{c_white_pieces_offset + c_pieces_region_size};
constexpr int c_black_to_move_region_size{1};

constexpr int c_castling_rights_offset{c_black_to_move_offset + c_black_to_move_region_size};
constexpr int c_castling_rights_region_size{16};

constexpr int c_en_passant_file_offset{c_castling_rights_offset + c_castling_rights_region_size};
[[maybe_unused]] constexpr int c_en_passant_region_size{8};

} // namespace

const std::array<zhash_t, 793> Zobrist_hash::m_random_numbers{
  198662566ULL,  1019954164ULL, 438798268ULL,  1323508509ULL, 769321038ULL,  648882693ULL,  1723287216ULL,
  1394360203ULL, 2091365417ULL, 743191265ULL,  1764880937ULL, 1667109176ULL, 482433522ULL,  1023559821ULL,
  1796338428ULL, 1689133694ULL, 1964611777ULL, 383633685ULL,  279653100ULL,  571967394ULL,  190628988ULL,
  1567444157ULL, 1861022199ULL, 1811739298ULL, 564137907ULL,  1134453953ULL, 1247300180ULL, 372794996ULL,
  406547693ULL,  719729778ULL,  959491377ULL,  1811994941ULL, 2004161408ULL, 1227268504ULL, 1629218749ULL,
  1104411664ULL, 1809351722ULL, 1335063226ULL, 2007648641ULL, 1087053580ULL, 1897574585ULL, 90739575ULL,
  246094444ULL,  1679260595ULL, 1042482484ULL, 296133474ULL,  1769155235ULL, 1159255129ULL, 1211572365ULL,
  389207094ULL,  1566251993ULL, 1049963724ULL, 505298674ULL,  1542041395ULL, 1584039948ULL, 1473485229ULL,
  1051491641ULL, 709475936ULL,  1069904564ULL, 496364540ULL,  342663564ULL,  1305080885ULL, 1425565593ULL,
  264225022ULL,  435236723ULL,  1723930095ULL, 1785261178ULL, 1836076458ULL, 1117209822ULL, 1970746928ULL,
  1811960882ULL, 1482293344ULL, 958239638ULL,  405202093ULL,  1948302217ULL, 1494427663ULL, 996529647ULL,
  481060668ULL,  441324976ULL,  386064583ULL,  314344697ULL,  250009454ULL,  1856045853ULL, 1628447669ULL,
  1058179718ULL, 264701432ULL,  469378527ULL,  6959450ULL,    44548546ULL,   1703918494ULL, 1825028200ULL,
  1009310402ULL, 2034904012ULL, 2085653824ULL, 1862219729ULL, 1200447443ULL, 709076590ULL,  1568250874ULL,
  1036748605ULL, 1902106868ULL, 109141117ULL,  100315712ULL,  737317457ULL,  1162642154ULL, 713076160ULL,
  1842281677ULL, 267931825ULL,  328866410ULL,  607425880ULL,  1061176882ULL, 760454484ULL,  2008254209ULL,
  964548533ULL,  491591394ULL,  2050437420ULL, 242055103ULL,  962473264ULL,  1254658999ULL, 1238155057ULL,
  1338349432ULL, 94333218ULL,   803439917ULL,  963044521ULL,  546884851ULL,  709060608ULL,  1654773539ULL,
  525366794ULL,  945863922ULL,  978211297ULL,  934036319ULL,  179015452ULL,  1303291211ULL, 1895294876ULL,
  150776163ULL,  138237276ULL,  1750364705ULL, 173910158ULL,  166589918ULL,  1498711192ULL, 1112782311ULL,
  1509013702ULL, 1481769310ULL, 17947770ULL,   789003253ULL,  168654633ULL,  834221979ULL,  603288765ULL,
  98861203ULL,   336403103ULL,  1038895977ULL, 59529292ULL,   1344980360ULL, 1280193312ULL, 1068551821ULL,
  2000915915ULL, 1673919491ULL, 2088885335ULL, 31209786ULL,   1834028327ULL, 1690278798ULL, 1479474005ULL,
  776001200ULL,  1153191313ULL, 43886871ULL,   737319509ULL,  320268003ULL,  1664198355ULL, 380708701ULL,
  2042902571ULL, 1408845230ULL, 764916234ULL,  1314492324ULL, 970343705ULL,  1247584207ULL, 1063956394ULL,
  2093882387ULL, 422216384ULL,  855934086ULL,  1856823109ULL, 1053107358ULL, 1558014760ULL, 1762134761ULL,
  1761402245ULL, 1456466407ULL, 273830527ULL,  330191081ULL,  1810676206ULL, 756424987ULL,  796899150ULL,
  1271349621ULL, 697503422ULL,  712059529ULL,  256523451ULL,  798585634ULL,  809429374ULL,  1963838281ULL,
  984572388ULL,  1837014095ULL, 2101231648ULL, 1728003396ULL, 768434287ULL,  1209145501ULL, 642144819ULL,
  1832294746ULL, 1587014827ULL, 474907853ULL,  1731917353ULL, 1017911393ULL, 13189201ULL,   1327793560ULL,
  68672901ULL,   363800186ULL,  124105918ULL,  2116028189ULL, 1618882307ULL, 848310886ULL,  807841919ULL,
  1526201390ULL, 1391317801ULL, 697043706ULL,  1243833167ULL, 1429685769ULL, 886713284ULL,  272365896ULL,
  1306655437ULL, 1114379727ULL, 1160351147ULL, 281987233ULL,  289558117ULL,  1688207296ULL, 781729287ULL,
  1527113556ULL, 47628297ULL,   1822190339ULL, 1466360874ULL, 945724625ULL,  1639333ULL,    1591199508ULL,
  79441513ULL,   1526272089ULL, 1341260041ULL, 1198329179ULL, 2133959254ULL, 1418706483ULL, 1839553818ULL,
  529387982ULL,  997850966ULL,  967635065ULL,  304614600ULL,  196565048ULL,  1177679707ULL, 1976296671ULL,
  1441760058ULL, 113975771ULL,  1100147886ULL, 1955744282ULL, 1900643102ULL, 1404696414ULL, 491177717ULL,
  22157061ULL,   1255299034ULL, 1221055956ULL, 141813808ULL,  1834946803ULL, 121057793ULL,  14331142ULL,
  1285291680ULL, 2028322412ULL, 975926856ULL,  155502584ULL,  1915617569ULL, 1389350390ULL, 1460282259ULL,
  1041934801ULL, 450969975ULL,  463812393ULL,  1228176199ULL, 1779968796ULL, 1559834952ULL, 710202241ULL,
  1134200244ULL, 1157814384ULL, 1665489978ULL, 883436922ULL,  1743808643ULL, 372044526ULL,  1313673666ULL,
  1896031746ULL, 1449721786ULL, 1321914528ULL, 1911972401ULL, 125541737ULL,  307747509ULL,  528321776ULL,
  1136318517ULL, 1129348429ULL, 1002343809ULL, 1194268323ULL, 1586079174ULL, 945500331ULL,  1355595406ULL,
  1629424057ULL, 861304472ULL,  1237293383ULL, 317210355ULL,  451046897ULL,  1358063275ULL, 990319477ULL,
  185609736ULL,  232643494ULL,  325394201ULL,  750499316ULL,  1245739616ULL, 853034830ULL,  1705847331ULL,
  1208100031ULL, 1393417910ULL, 1707447085ULL, 500238173ULL,  1235094418ULL, 1425413657ULL, 2006104359ULL,
  1684646804ULL, 210473880ULL,  1242491249ULL, 206158743ULL,  637515700ULL,  495781916ULL,  1305113340ULL,
  1684287233ULL, 18438317ULL,   710072658ULL,  1707958263ULL, 1574710742ULL, 984605459ULL,  2056352025ULL,
  1171734761ULL, 1698018007ULL, 1464667366ULL, 1089998013ULL, 1994611411ULL, 488069209ULL,  1181915965ULL,
  817263926ULL,  589704250ULL,  1603519617ULL, 1121692840ULL, 230149514ULL,  45930636ULL,   157781395ULL,
  1937234667ULL, 966862218ULL,  845627288ULL,  1821602238ULL, 416267625ULL,  1243161170ULL, 1998654905ULL,
  778802862ULL,  1897203200ULL, 1274246196ULL, 1733432329ULL, 604883706ULL,  180540153ULL,  917077534ULL,
  1423135902ULL, 1313225749ULL, 67149807ULL,   1309728123ULL, 1868199857ULL, 254483878ULL,  967360818ULL,
  62924232ULL,   1914143492ULL, 368592464ULL,  1100035265ULL, 153967887ULL,  1940064583ULL, 1383758338ULL,
  421353041ULL,  1767788248ULL, 1527691487ULL, 265880412ULL,  710939403ULL,  995440894ULL,  382914899ULL,
  1464330818ULL, 559858588ULL,  429191027ULL,  459035511ULL,  2017702931ULL, 1721382842ULL, 740107673ULL,
  193843649ULL,  866016736ULL,  996129647ULL,  1320130010ULL, 24920143ULL,   1218649112ULL, 1560275958ULL,
  266905172ULL,  1090440916ULL, 141171642ULL,  841282564ULL,  1203563313ULL, 1722564106ULL, 1324695547ULL,
  612817625ULL,  1082368795ULL, 353733705ULL,  834663305ULL,  1953712482ULL, 1546594123ULL, 1401691297ULL,
  1733957506ULL, 691996104ULL,  1837282513ULL, 1619626099ULL, 1395029302ULL, 1672594287ULL, 106852461ULL,
  1021288966ULL, 1608450418ULL, 34567896ULL,   1787645982ULL, 1930687285ULL, 1835849391ULL, 617450531ULL,
  97135892ULL,   243930631ULL,  810027168ULL,  14112041ULL,   1543291691ULL, 267370796ULL,  1624726724ULL,
  824733638ULL,  319802447ULL,  1170263770ULL, 1577720878ULL, 1787358122ULL, 1447977637ULL, 133033522ULL,
  1990040508ULL, 269649092ULL,  1120560287ULL, 70917646ULL,   1279486927ULL, 1594981901ULL, 1864259341ULL,
  1408228602ULL, 1779106288ULL, 963550648ULL,  80891415ULL,   1724884264ULL, 1466070089ULL, 353335940ULL,
  938667955ULL,  1905747270ULL, 913542824ULL,  1730954325ULL, 7664979ULL,    1597522879ULL, 74947595ULL,
  437148112ULL,  1771553303ULL, 1507376591ULL, 1032867617ULL, 859386746ULL,  1659292142ULL, 214333344ULL,
  1243793869ULL, 413267728ULL,  604874323ULL,  636955578ULL,  1980809002ULL, 838799960ULL,  288992399ULL,
  1472400791ULL, 777631874ULL,  1829404356ULL, 1750393419ULL, 2074296166ULL, 2030427252ULL, 48590576ULL,
  1645948850ULL, 1750071744ULL, 1816989922ULL, 141918570ULL,  1713604637ULL, 452411005ULL,  996922515ULL,
  1041687846ULL, 527481081ULL,  287521127ULL,  2070953081ULL, 1411274837ULL, 571094577ULL,  1430464900ULL,
  771231823ULL,  1046152292ULL, 34018323ULL,   540794816ULL,  945556884ULL,  957148245ULL,  1086158931ULL,
  2008180337ULL, 1798985054ULL, 1429616737ULL, 1444807551ULL, 972917983ULL,  708164189ULL,  206581556ULL,
  1416706778ULL, 353554950ULL,  1307108526ULL, 1953451707ULL, 464398911ULL,  1077846017ULL, 1437658378ULL,
  715809148ULL,  368587976ULL,  280321192ULL,  758255153ULL,  132333151ULL,  120974626ULL,  1101135407ULL,
  449895205ULL,  1553805673ULL, 960429874ULL,  693210760ULL,  1673014489ULL, 721990145ULL,  551765533ULL,
  958930021ULL,  149746647ULL,  1156319983ULL, 1228107764ULL, 252499430ULL,  239564552ULL,  526120982ULL,
  554498768ULL,  1650558465ULL, 1854613083ULL, 1694093296ULL, 600471000ULL,  367063099ULL,  388738474ULL,
  894405035ULL,  2008810949ULL, 456811775ULL,  1372693772ULL, 1453356644ULL, 1922032787ULL, 1697868796ULL,
  2059430347ULL, 576058739ULL,  1171113961ULL, 197488138ULL,  1371338093ULL, 6403949ULL,    1187799876ULL,
  612344703ULL,  357069996ULL,  845912868ULL,  1603272730ULL, 252966445ULL,  1036665582ULL, 769818354ULL,
  1356141643ULL, 450083467ULL,  1774430389ULL, 196973988ULL,  2107837348ULL, 1316562638ULL, 1143309237ULL,
  2108272547ULL, 617314907ULL,  209344924ULL,  934255147ULL,  197349476ULL,  296964340ULL,  1154836458ULL,
  830407731ULL,  927100548ULL,  732487622ULL,  606078234ULL,  1001340300ULL, 1569133433ULL, 1495378731ULL,
  1134177959ULL, 168234654ULL,  1241705959ULL, 1633312831ULL, 809475820ULL,  222611238ULL,  385398798ULL,
  1187900792ULL, 1364658932ULL, 387010411ULL,  1300452930ULL, 1790949249ULL, 1500210735ULL, 1469541772ULL,
  1533836332ULL, 963404662ULL,  1414331006ULL, 643809546ULL,  1935687444ULL, 1919243319ULL, 6031022ULL,
  245852969ULL,  751181333ULL,  921023801ULL,  837390287ULL,  2107848729ULL, 602951260ULL,  1034280872ULL,
  634372497ULL,  1056619170ULL, 216769944ULL,  1100253714ULL, 1394103158ULL, 2117714498ULL, 654106909ULL,
  516324225ULL,  867319926ULL,  114202089ULL,  313521502ULL,  254813682ULL,  500831176ULL,  888160127ULL,
  933643992ULL,  46299096ULL,   238725880ULL,  188034119ULL,  1422379837ULL, 1048628228ULL, 110079203ULL,
  1666343423ULL, 1156536721ULL, 323592083ULL,  1323906678ULL, 1922143680ULL, 377099388ULL,  1273502839ULL,
  2103738268ULL, 1476176474ULL, 1276696283ULL, 392945606ULL,  1707162726ULL, 81706692ULL,   1013112209ULL,
  140739857ULL,  2079718572ULL, 1744266586ULL, 275816813ULL,  1421371308ULL, 761213517ULL,  705890796ULL,
  816711373ULL,  243494480ULL,  2093715180ULL, 597867736ULL,  27599610ULL,   488461851ULL,  1334269979ULL,
  1412315488ULL, 523700585ULL,  138634631ULL,  740903709ULL,  1440347456ULL, 1807523012ULL, 218521672ULL,
  445222729ULL,  2069348507ULL, 752057155ULL,  2021627876ULL, 737520939ULL,  186338855ULL,  1260026560ULL,
  1080194051ULL, 236098796ULL,  511275762ULL,  212977190ULL,  141810173ULL,  989208554ULL,  1773576745ULL,
  77108457ULL,   420241980ULL,  332922693ULL,  825884781ULL,  1229114585ULL, 1489043005ULL, 170875761ULL,
  251167322ULL,  1821968771ULL, 1355886404ULL, 858225303ULL,  2058984598ULL, 240517920ULL,  1379042896ULL,
  1402758859ULL, 185105844ULL,  1489918200ULL, 402147378ULL,  106965102ULL,  937565229ULL,  493181942ULL,
  79823519ULL,   1786665966ULL, 931666555ULL,  1815196113ULL, 560174775ULL,  1208473038ULL, 11146929ULL,
  597706448ULL,  1303344356ULL, 737757652ULL,  480404028ULL,  1480058745ULL, 910310921ULL,  1715482926ULL,
  1855936091ULL, 971652881ULL,  1084424731ULL, 1618997880ULL, 1663133228ULL, 2050161355ULL, 391895464ULL,
  2021536707ULL, 1471262621ULL, 499859784ULL,  1704397202ULL, 1030216177ULL, 1873283215ULL, 1205540465ULL,
  613870265ULL,  857775561ULL,  2104895908ULL, 1657360687ULL, 811901736ULL,  381535161ULL,  1726704699ULL,
  1255348525ULL, 1483879280ULL, 1170615101ULL, 1476977125ULL, 1302418932ULL, 322492675ULL,  245843096ULL,
  2100194691ULL, 374708367ULL,  2085663338ULL, 1193403907ULL, 1646232978ULL, 407483519ULL,  1773669154ULL,
  518062881ULL,  1858971984ULL, 484331528ULL,  1403466435ULL, 1235002448ULL, 416845965ULL,  1492051450ULL,
  422964988ULL,  1731838779ULL, 859563480ULL,  822762443ULL,  932754531ULL,  904344947ULL,  1930519594ULL,
  123753607ULL,  1759053243ULL, 975062311ULL,  1299007357ULL, 167171935ULL,  650737498ULL,  1119779950ULL,
  956137790ULL,  1579757891ULL, 360787682ULL,  899859132ULL,  1319205303ULL, 1645356814ULL, 548569777ULL,
  665243259ULL,  793816958ULL,  831845962ULL,  1183915673ULL, 1194473633ULL, 1267490706ULL, 216312319ULL,
  388117189ULL,  1220372050ULL};

Zobrist_hash::Zobrist_hash(Board const& board)
{
  constexpr static std::array piece_types{Piece::pawn, Piece::knight, Piece::bishop,
                                          Piece::rook, Piece::queen,  Piece::king};
  for (auto piece : piece_types)
  {
    auto const piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
    for (auto const sq_index : board.get_piece_set(Color::black, piece))
    {
      auto const rand_index = c_black_pieces_offset + (c_board_dimension_squared * piece_offset) + sq_index;
      m_hash ^= m_random_numbers[rand_index];
    }
    for (auto const sq_index : board.get_piece_set(Color::white, piece))
    {
      auto const rand_index = c_white_pieces_offset + (c_board_dimension_squared * piece_offset) + sq_index;
      m_hash ^= m_random_numbers[rand_index];
    }
  }

  if (board.get_active_color() == Color::black)
  {
    update_player_to_move();
  }

  update_en_passant_square(board.get_en_passant_square());
  update_castling_rights(board.get_castling_rights());
}

void Zobrist_hash::update_en_passant_square(Bitboard ep_square)
{
  if (!ep_square.is_empty())
  {
    m_hash ^= m_random_numbers[c_en_passant_file_offset + Coordinates{ep_square.bitscan_forward()}.y()];
  }
}

void Zobrist_hash::update_castling_rights(Castling_rights rights)
{
  // Castling rights is packed into a uint8, so reinterpret casting to uint will give us a value 0 <= val < 16
  static_assert(sizeof(Castling_rights) == 1);

  //NOLINTNEXTLINE
  auto const as_uint8 = 0x0f & *reinterpret_cast<uint8_t*>(&rights);
  MY_ASSERT(as_uint8 < c_castling_rights_region_size, "Value is not a valid castling rights object");

  m_hash ^= m_random_numbers[c_castling_rights_offset + as_uint8];
}

void Zobrist_hash::update_piece_location(Color color, Piece piece, Coordinates location)
{
  // This array can be indexed by color
  constexpr static std::array piece_offsets{c_black_pieces_offset, c_white_pieces_offset};

  auto const piece_offset = static_cast<uint8_t>(piece) - static_cast<uint8_t>(Piece::pawn);
  auto const rand_index_base = piece_offsets[static_cast<uint8_t>(color)] + (c_board_dimension_squared * piece_offset);
  m_hash ^= m_random_numbers[rand_index_base + location.square_index()];
}

void Zobrist_hash::update_player_to_move()
{
  m_hash ^= m_random_numbers[c_black_to_move_offset];
}

zhash_t Zobrist_hash::get_hash() const
{
  return m_hash;
}

std::ostream& operator<<(std::ostream& os, Zobrist_hash const& self)
{
  os << std::to_string(self.get_hash());
  return os;
}
