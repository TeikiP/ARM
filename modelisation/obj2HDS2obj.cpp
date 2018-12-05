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
typedef Kernel::Vector_3 Vector_3;

typedef Polyhedron::Vertex Vertex;
typedef Polyhedron::Vertex_iterator Vertex_iterator;

typedef Polyhedron::Halfedge_iterator Halfedge_iterator;

typedef Polyhedron::Halfedge_handle Halfedge_handle;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;
typedef Polyhedron::Halfedge_around_vertex_circulator Halfedge_vertex_circulator;

typedef Polyhedron::Facet_iterator Facet_iterator;

struct Plane_equation {
    template <class Facet>
    typename Facet::Plane_3 operator()( Facet& f) {
        typename Facet::Halfedge_handle h = f.halfedge();
        typedef typename Facet::Plane_3  Plane;
        return Plane( h->vertex()->point(),
                      h->next()->vertex()->point(),
                      h->next()->next()->vertex()->point());
    }
};


using namespace std;

void fillHole(Polyhedron& P);
void fillHoleTriangle(Polyhedron& P);
void fillHoleCenter(Polyhedron& P);
void write_obj(const char* file_name, Polyhedron P);
void widen(Polyhedron &P, double d);
void build_faces(Polyhedron& P, vector<vector<vector<Vertex>>> face);

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

  std::transform( P.facets_begin(), P.facets_end(), P.planes_begin(), Plane_equation());

    std::cout << "----------FILLING HOLE---------------" << std::endl;
  fillHole(P);
//    fillHoleTriangle(P);
//    fillHoleCenter(P);
//    std::cout << "----------WIDEN---------------" << std::endl;
//    widen(P,1.5);

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
    Halfedge_iterator hole, h, g, tmp;
    for (hole = P.border_halfedges_begin(); hole !=  P.halfedges_end(); ++hole)
    {
        if (hole->is_border()) {
						//TODO
						//No fill holes
						//No split facet
						//Use add facet to border

            P.fill_hole(hole);
            std::cout << "Face created" << std::endl;
            Plane_3 plan = Plane_3(hole->vertex()->point(), hole->next()->vertex()->point(), hole->prev()->vertex()->point());

            Vector_3 normal = plan.orthogonal_vector();
            Vector_3 n1 = hole->opposite()->facet()->plane().orthogonal_vector();
            Vector_3 n2 = hole->next()->opposite()->facet()->plane().orthogonal_vector();
            Vector_3 n3 = hole->prev()->opposite()->facet()->plane().orthogonal_vector();
            while( cross_product(normal, n1) == Vector_3(CGAL::NULL_VECTOR) || cross_product(normal, n2) == Vector_3(CGAL::NULL_VECTOR) || cross_product(normal, n2) == Vector_3(CGAL::NULL_VECTOR)){
                hole = hole->next();
                plan = Plane_3(hole->vertex()->point(), hole->next()->vertex()->point(), hole->prev()->vertex()->point());
                n1 = hole->opposite()->facet()->plane().orthogonal_vector();
                n2 = hole->next()->opposite()->facet()->plane().orthogonal_vector();
                n3 = hole->prev()->opposite()->facet()->plane().orthogonal_vector();
            }

            h = hole->next();
            g = hole->prev();

            while(g != h) {
                while( plan.has_on(h->next()->vertex()->point()) && g != h->next() ){
                    h = h->next();
                }

                while( plan.has_on(g->prev()->vertex()->point()) && g->prev() != h ){
                    g = g->prev();
                }

                if(g == h || h->next() == g || g->next() == h)
                    break;

                h = P.split_facet(h, g);
                std::cout << "Face created" << std::endl;
                //  h = P.add_facet_to_border(h, g);
                h = h->opposite();
                plan = Plane_3(h->vertex()->point(), h->next()->vertex()->point(), h->prev()->vertex()->point());

                normal = plan.orthogonal_vector();
                n1 = hole->opposite()->facet()->plane().orthogonal_vector();
                n2 = hole->next()->opposite()->facet()->plane().orthogonal_vector();
                n3 = hole->prev()->opposite()->facet()->plane().orthogonal_vector();

                while( cross_product(normal, n1) == Vector_3(CGAL::NULL_VECTOR) || cross_product(normal, n2) == Vector_3(CGAL::NULL_VECTOR) || cross_product(normal, n2) == Vector_3(CGAL::NULL_VECTOR)){
                    h = h->next();
                    plan = Plane_3(h->vertex()->point(), h->next()->vertex()->point(), h->prev()->vertex()->point());
                    n1 = h->opposite()->facet()->plane().orthogonal_vector();
                    n2 = h->next()->opposite()->facet()->plane().orthogonal_vector();
                    n3 = h->prev()->opposite()->facet()->plane().orthogonal_vector();
                }

                g = h->prev();

            }
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

void widen(Polyhedron &P, double d)
{
    vector<vector<vector<Vertex>>> faces;
    int iFit=0;

    for(Facet_iterator fit = P.facets_begin(); fit != P.facets_end(); fit++, iFit++)
    {
        vector<vector<Vertex>> verts;
        Halfedge_facet_circulator hfc= fit->facet_begin();
        do{
          if(!hfc->is_border()) {
              Vertex_iterator v = hfc->vertex();

              Halfedge_vertex_circulator hvc = v->vertex_begin();

              Vector_3 normal = Vector_3(0.,0.,0.);
              int nbface = 0;

              do{
                  if(!hvc->is_border())
                  {
                      normal = normal + hvc->facet()->plane().orthogonal_vector();
                      nbface++;

                  }
                  hvc++;
              }while(hvc != v->vertex_begin());

              normal = normal / nbface;
              vector<Vertex> vert;
              vert.push_back(Vertex(v->point()));
              vert.push_back(Vertex(v->point() + normal * d ));
              verts.push_back(vert);
          }
          ++hfc;
        }while(hfc != fit->facet_begin());

        faces.push_back(verts);

    }

    build_faces(P, faces);
}

void build_faces(Polyhedron& P, vector<vector<vector<Vertex>>> face)
{


    for(int i=0; i<face.size(); ++i)
    {
        Point_3 A = face[i][0][1].point();
        std::cout << "A: "<<A << std::endl;


        for(int j=0; j<face[i].size()-2; ++j)
        {

            Point_3 B = face[i][j+1][1].point();
            std::cout << "B: "<<B << std::endl;
            Point_3 C = face[i][j+2][1].point();
            std::cout << "C: "<<C << std::endl;

            Halfedge_iterator h = P.make_triangle(A,B,C);
            std::cout << h->vertex()->point() << std::endl;
            std::cout << h->next()->vertex()->point() << std::endl;
            std::cout << h->next()->next()->vertex()->point() << std::endl;

        }
    }
}
