//  original source of BuildCgalPolyhedronFromObj downloaded from:
//  http://saurabhg.com/programming/wavefront-obj-reader-building-cgalpolyhedron_3/
//  ImportOBJ.h , TestImportOBJ.cpp

#include <CGAL/basic.h>
#include <CGAL/IO/File_header_OFF.h>
#include <CGAL/IO/File_scanner_OFF.h>
#include <CGAL/IO/File_writer_OFF.h>
#include <CGAL/IO/File_writer_wavefront.h>
#include <CGAL/IO/File_writer_wavefront_impl.h>
#include <CGAL/Modifier_base.h>
#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Cartesian.h>
#include <CGAL/IO/Polyhedron_geomview_ostream.h>

#include <fstream>
#include <iostream>
#include <vector>

template <class HDS>
class BuildCgalPolyhedronFromObj : public CGAL::Modifier_base<HDS> {
private:
  std::istream& _file;
public:
  BuildCgalPolyhedronFromObj(std::istream& file) : _file(file) {}
  void
  operator() (HDS& hds)
  {
    typedef typename HDS::Vertex   Vertex;
    typedef typename Vertex::Point Point;
    if(!_file){ return;}
    std::string _line;
    int _numVertices = 0;
    int _numFacets   = 0;
    while(_file.good())
      {
	std::getline(_file, _line);
	if(_line.size() > 1)
	  {
	    if(_line[0]=='v' && _line[1]==' ') {++_numVertices;}
	    if(_line[0]=='f' && _line[1]==' ') {++_numFacets;}
	  }
      }
    if(!_file.good()){ _file.clear();}
    _file.seekg(0);
    // Postcondition: hds is a valid polyhedral surface.
    CGAL::Polyhedron_incremental_builder_3<HDS> B(hds, true);
    B.begin_surface(_numVertices, _numFacets, int((_numVertices + _numFacets - 2)*2.1));
    std::string _token;
    while(!_file.eof())
      {
	_token = ""; // Reset token.
	_file >> _token;
	// if token is v then its a vertex.
	if(_token=="v")
	  {
	    double x, y, z;
	    _file >> x >> y >> z;
	    B.add_vertex(Point(x, y, z));
	  }
	// There are 4 type of facets.
	// a     only vertex index.
	// a/b   vertex and texture index.
	// a/b/c vertex, texture and normal index.
	// a//c  vertex and normal index.
	else if(_token=="f")
	  {
	    // Read the remaining line for the facet.
	    std::string _line;
	    std::getline(_file, _line);
	    // Split the line into facet's vertices.
	    // The length of _vertices is equal to the number of vertices for this face.
	    std::istringstream _stream(_line);
	    std::vector<std::string> _vertices;
	    std::copy(std::istream_iterator<std::string>(_stream),
		      std::istream_iterator<std::string>(),
		      std::back_inserter(_vertices));
	    // For each vertex read only the first number, which is the vertex index.
	    B.begin_facet();
	    for(size_t i=0 ; i<_vertices.size() ; ++i)
	      {
		std::string::size_type _pos = _vertices[i].find('/', 0);
		std::string _indexStr = _vertices[i].substr(0, _pos);
		B.add_vertex_to_facet(stoi(_indexStr)-1); // -1 is because OBJ file uses 1 based index.
	      }
	    B.end_facet();
	  }
      }

    B.end_surface();
    if(B.error()){
      std::cout << " BuildCgalPolyhedronFromObj operator() : input error " << std::endl ;
      std::cout << " obj2HDS2obj Failure " << std::endl ;
      exit(1);
    }
  }
};

typedef CGAL::Simple_cartesian<double> Kernel;

typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::HalfedgeDS HalfedgeDS;


typedef Kernel::Plane_3 Plane_3;
typedef Kernel::Point_3 Point_3;

typedef Polyhedron::Vertex Vertex;
typedef Polyhedron::Vertex_iterator Vertex_iterator;

typedef Polyhedron::Halfedge_iterator Halfedge_iterator;

typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;
typedef Polyhedron::Halfedge_around_vertex_circulator Halfedge_vertex_circulator;

typedef Polyhedron::Facet_iterator Facet_iterator;

using namespace std;

void fillHole(Polyhedron& P);
void fillHoleTriangle(Polyhedron& P);
void fillHoleCenter(Polyhedron& P);
void write_obj(const char* file_name, Polyhedron P);

int
main(int argc, char* argv[])
{
  if(argc != 3)
    {
      std::cerr << "Usage: "<< argv[0]
		<< " <input_file.OBJ>  <output_file.OBJ> " <<std::endl;
      exit( 1);
    }

  std::ifstream f_in;
  f_in.open(argv[1]);
  std::ifstream* p_f_in= &f_in;
  if (!*p_f_in) {
    std::cerr << "error: cannot open file '"<< argv[1]
              << "' for reading." <<std::endl;
    exit( 1);
  }
  std::istream& ref_f_in= f_in;

  Polyhedron P;
  //----------------------------------- import from OBJ ---------------------
  BuildCgalPolyhedronFromObj<HalfedgeDS> P_scanned(ref_f_in);

  //----------------------------------- construct valid polyhedron
  P.delegate(P_scanned);
  //----------------------------------- CGAL tests ----------------------

  std::cout<< " is valid? (combinatorial consistency) " << P.is_valid() << std::endl;
  std::cout<< " is closed? " << P.is_closed() << std::endl;
  P.normalize_border();
  std::cout<< " number of border halfedges: " << P.size_of_border_halfedges() << std::endl;

  //----------------------------------- Fill Hole -------------------

	std::cout << "----------FILLING HOLE---------------" << std::endl;
  fillHole(P);
//    fillHoleTriangle(P);
//    fillHoleCenter(P);

  //----------------------------------- CGAL tests ----------------------

  std::cout<< " is valid? (combinatorial consistency) " << P.is_valid() << std::endl;
  std::cout<< " is closed? " << P.is_closed() << std::endl; //
  P.normalize_border();
  std::cout<< " number of border halfedges: " << P.size_of_border_halfedges() << std::endl;


  //----------------------------------- export in OBJ ---------------------------
  write_obj(argv[2], P);
  // ---------------------------------

  // CGAL::Geomview_stream geo;
  // geo << CGAL::GREEN << P;
  // Point_3 click;
  // geo >> click;

  f_in.close();
  std::cout << " obj2HDS2OBJ Success  " << std::endl ;
  return 0;
}


void
write_obj(const char* file_name, Polyhedron P)
{
  if(file_name == NULL)
    file_name= "./write_obj_output.obj";
  const char*  oname = "cout";
  ofstream     out;
  out.open(file_name);
  ostream* p_f_out = &out;

  if ( !*p_f_out) {
    cerr << file_name << ": error: cannot open file '"<< oname
	 << "' for writing." <<endl;
  }
  CGAL::File_writer_wavefront  writer;
  writer.write_header( out, P.size_of_vertices(), P.size_of_halfedges(), P.size_of_facets());

  std::map<const Vertex*,std::size_t, std::less<const Vertex*> > mapping;
  std::size_t vn = 0;
  Vertex_iterator vi= P.vertices_begin();
  for ( ; vi != P.vertices_end(); ++vi) {
    writer.write_vertex(vi->point().x(), vi->point().y(), vi->point().z());
    mapping[ &*vi] = vn;
    vn++;
  }
  writer.write_facet_header();
  Facet_iterator fi= P.facets_begin();
  for( ; fi != P.facets_end(); ++fi){
    writer.write_facet_begin( fi->facet_degree());
    Halfedge_facet_circulator hei= fi->facet_begin();
    do{
      CGAL_assertion( mapping.find(&*(hei->vertex())) != mapping.end());
      writer.write_facet_vertex_index(mapping[ &*(hei->vertex())]);
      ++hei;
    }while(hei != fi->facet_begin());
    writer.write_facet_end();
  }
  writer.write_footer();
  if ( !*p_f_out) {
    cerr << file_name << " write error: while writing file '"
	 << oname << "'." << endl;
    exit( 1);
  }
  out.close();
}

void fillHole(Polyhedron& P)
 {
    Halfedge_iterator h, g, tmp;
    for (h = P.border_halfedges_begin(); h !=  P.halfedges_end(); ++h)
    {
        if (h->is_border()) {
            P.fill_hole(h);
//            const Point_3 a = h->vertex()->point();
//            g = h->next();
//            const Point_3 b = g->vertex()->point();
//            g = g->next();
//            const Point_3 c = g->vertex()->point();
//            Plane_3 plan = Plane_3(a, b, c);
//            tmp = g;
//            g = g->next();
//            while(g != h){
//                if(!plan.has_on(g->vertex()->point()) ){
//                    h = P.split_facet(h, tmp);
//                }
//                tmp = g;
//                g = g->next();
//            }
        }
    }
}

void fillHoleTriangle(Polyhedron& P)
{
    Halfedge_iterator h, g;

    for (h = P.border_halfedges_begin(); h !=  P.halfedges_end(); ++h)
    {
        if (h->is_border()) {
            g = h->next()->next();
            while (g != h){
                P.add_facet_to_border(h, g);
                g = h->next()->next();
            }
        }
    }
}

void fillHoleCenter(Polyhedron& P)
 {
    for (Halfedge_iterator it = P.border_halfedges_begin(); it !=  P.halfedges_end(); ++it)
    {
        if (it->is_border()) {
            P.fill_hole(it);
            
						std::vector<Point_3> coords;
						
						Facet_iterator face = it->facet();
						unsigned int degree = face->facet_degree();
						
						Halfedge_facet_circulator circulator = face->facet_begin();
						Halfedge_facet_circulator start = face->facet_begin();
						
						do {
						    coords.push_back(circulator->vertex()->point());
						} while (++circulator != start);
						
						float xMean = 0, yMean = 0, zMean = 0;
						
						for (int i = 0; i < coords.size(); i++) {
						    xMean += coords.at(i)[0];
						    yMean += coords.at(i)[1];
						    zMean += coords.at(i)[2];
						}
						
				    xMean /= coords.size();
				    yMean /= coords.size();
				    zMean /= coords.size();				    
						
						Point_3 barycenter = Point_3(xMean, yMean, zMean);
						
						std::cout << "Barycenter = " << barycenter << std::endl;

						Halfedge_iterator centerEdge = P.create_center_vertex(it);
						centerEdge->vertex()->point() = barycenter;
						
						std::cout << "---------------------------" << std::endl;
				}
    }
}
