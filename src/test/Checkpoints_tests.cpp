//
// Unit tests for block-chain checkpoints
//
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "../checkpoints.h"
#include "../util.h"

using namespace std;

BOOST_AUTO_TEST_SUITE(Checkpoints_tests)

BOOST_AUTO_TEST_CASE(sanity)
{
    uint256 p11111 = uint256("0x05cbc42d0421207a39427f1b44610a30380e0e3dd593c154276d7c8d4ae8c9b2");
    uint256 p16666 = uint256("0x682d99d096441e046dae8dd86d5bdfaa32d4e29dea6cf38273bb2b261905f1e2");
    BOOST_CHECK(Checkpoints::CheckBlock(11111, p11111));
    BOOST_CHECK(Checkpoints::CheckBlock(16666, p16666));

    
    // Wrong hashes at checkpoints should fail:
    BOOST_CHECK(!Checkpoints::CheckBlock(11111, p16666));
    BOOST_CHECK(!Checkpoints::CheckBlock(16666, p11111));

    // ... but any hash not at a checkpoint should succeed:
    BOOST_CHECK(Checkpoints::CheckBlock(11111+1, p16666));
    BOOST_CHECK(Checkpoints::CheckBlock(16666+1, p11111));

    BOOST_CHECK(Checkpoints::GetTotalBlocksEstimate() >= 16666);
}    

BOOST_AUTO_TEST_SUITE_END()
