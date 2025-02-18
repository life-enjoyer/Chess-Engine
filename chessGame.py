import ctypes
from sys import platform


class Coordinates(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_uint),
        ("y", ctypes.c_uint)
    ]


class Piece(ctypes.Structure):
    _fields_ = [
        ("coordinates", Coordinates),
        ("piece", ctypes.c_ubyte)
    ]


class KingRelatedSquares(ctypes.Structure):
    __fields__ = [("kingPosition", ctypes.c_uint),
                  ("linesRelated", ctypes.POINTER(ctypes.c_uint))]

    """Each line position is an unsigned int
    the first position is the minimal column to be concerned, the second the maximal, the third the minimal line and the forth the maximal
    the fifth is the minimal first diagonal, the sixth the maximal, the seventh the minimal second diagonal and the eighth the maximal
                                                                                                                           """


class BoardState(ctypes.Structure):
    _fields_ = [("pieces", ctypes.POINTER(Piece)),
                ("piecesSize", ctypes.c_uint),
                ("checked", ctypes.c_byte),
                ("gameState", ctypes.c_byte),
                ("whiteKingsRelatedSquares", ctypes.POINTER(KingRelatedSquares)),
                ("blackKingRelatedSquares", ctypes.POINTER(KingRelatedSquares))]


class StatesRequest(ctypes.Structure):
    _fields_ = [("currentBoardState", BoardState),
                ("boardStateSize", ctypes.POINTER(ctypes.c_uint)),
                ("turnOf", ctypes.c_int)]


if platform == "win32":
    library = ctypes.windll.LoadLibrary('engine.dll')
else:
    library = ctypes.CDLL('./engine.so')

"""print_piece_array function"""
library.print_pieces_array.argtypes = [ctypes.POINTER(Piece), ctypes.c_int]

"""print_board_state function"""
library.print_board_state.argtypes = [BoardState]

"""get_all_possible_next_board_states function"""
library.get_all_possible_next_board_states.restype = ctypes.POINTER(BoardState)
library.get_all_possible_next_board_states.argtypes = [StatesRequest]

"""get_start_board function"""
library.get_start_board.restype = BoardState


def get_start_board_state():
    return library.get_start_board()


def get_next_board_states(current_board_state):
    board_state_size = ctypes.c_uint(0)
    request = StatesRequest(current_board_state, ctypes.pointer(board_state_size), 0)

    next_board_states = library.get_all_possible_next_board_states(request)

    next_board_states_python_array = []

    for i in range(board_state_size.value):
        next_board_states_python_array.append(next_board_states[i])

    return next_board_states_python_array


"""This is a test function that prints all the nodes from the starting position"""
def print_all_boards_from_starting_position():
    currentBoardState = library.get_start_board()
    boardStatesSize = ctypes.c_uint(0)

    request = StatesRequest(currentBoardState, ctypes.pointer(boardStatesSize), 0)

    all_boards = library.get_all_possible_next_board_states(request)

    for i in range(boardStatesSize.value):
        library.print_board_state(all_boards[i])

    print(boardStatesSize.value)

    return all_boards
